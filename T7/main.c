//
// Created by Des Caldnd on 12/1/2023.
//
#include "../structs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

enum command_type
{ FIND, EDIT, INSERT, REMOVE, PRINT, PRINT_ALL, UNDO, QUIT, UNDEF };

enum key_type
{ SURNAME, NAME, FATHERNAME, DATE, SALARY, UNDEFINED };

struct Date
{
    size_t day, month, year;
};

struct Liver
{
    struct String surname, name, fathername;
    struct Date b;
    char sex;
    double salary;
};

LIST(liver, struct Liver)

union Backup
{
    struct String string;
    struct Date date;
    double salary;
};

struct Edit_t
{
    enum key_type key;
    size_t id;
    union Backup backup;
};

struct Insert_t
{
    size_t id;
};

struct Remove_t
{
    struct Liver backup;
};

union Op_data
{
    struct Edit_t edit;
    struct Insert_t insert;
    struct Remove_t remove;
};

struct Operation
{
    union Op_data data;
    void (*undo)(union Op_data* data, struct list_liver*);
    void (*delete)(union Op_data* data);
};

DEQUE(oper, struct Operation)


void parse_file(struct list_liver* list, FILE* in);
int less_age(struct Liver lhs, struct Liver rhs);
void destroy_liver(struct Liver* liver, void*);
struct node_liver* insert_liver(struct list_liver* list, struct Liver liver);
int parse_liver(struct Liver* liver, FILE* in);
void print_liver(struct Liver* liver, void*);
size_t get_id(struct list_liver* list, struct node_liver* node);

void find(struct list_liver* list, struct deque_oper* deque);
void insert(struct list_liver* list, struct deque_oper* deque);
void print(struct list_liver* list);
void undo(struct deque_oper* deque, struct list_liver*);
void edit(struct node_liver* liver, struct list_liver* list, struct deque_oper* deque);
void remove_(struct node_liver* liver, struct list_liver* list, struct deque_oper* deque);

int surname_eq(struct Liver lhs, struct Liver rhs);
int name_eq(struct Liver lhs, struct Liver rhs);
int fathername_eq(struct Liver lhs, struct Liver rhs);
int date_eq(struct Liver lhs, struct Liver rhs);
int salary_eq(struct Liver lhs, struct Liver rhs);

void push_operation(struct deque_oper* deque, struct Operation);
void destroy_undo(struct Operation* oper, void*);
void undo_edit(union Op_data* data, struct list_liver* list);
void undo_insert(union Op_data* data, struct list_liver* list);
void undo_remove(union Op_data* data, struct list_liver* list);
void delete_edit(union Op_data* data);
void delete_insert(union Op_data* data);
void delete_remove(union Op_data* data);

enum command_type get_command_type(char* str);
enum key_type get_key_type(char* str);

size_t N = 0;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Incorrect argc\n");
        return 0;
    }

    FILE *in = fopen(argv[1], "r");

    if (in == NULL)
    {
        printf("Cannot open file\n");
        return 0;
    }

    struct list_liver livers = init_list_liver();
    struct deque_oper deque = init_deque_oper(10);

    if (!is_list_valid_liver(livers) || !is_valid_deque_oper(&deque))
    {
        destroy_list_liver(&livers);
        destroy_deque_oper(&deque);
        printf("Bad alloc\n");
        fclose(in);
        return 0;
    }

    parse_file(&livers, in);
    fclose(in);

    if (!is_list_valid_liver(livers))
    {
        destroy_deque_oper(&deque);
        printf("Bad alloc\n");
        return 0;
    }

    char command[20];
    printf("Enter command: ");
    scanf("%19s", command);
    enum command_type type = get_command_type(command);

    while (type != QUIT)
    {
        switch (type)
        {
            case FIND:
            {
                find(&livers, &deque);
                break;
            }
            case INSERT:
            {
                insert(&livers, &deque);
                break;
            }
            case PRINT:
            {
                print(&livers);
                break;
            }
            case PRINT_ALL:
            {
                for_all_list_liver(livers, &print_liver, NULL);
                break;
            }
            case UNDO:
            {
                undo(&deque, &livers);
                break;
            }
            default:
            {
                printf("Incorrect command\n");
            }
        }

        if (!is_list_valid_liver(livers) || !is_valid_deque_oper(&deque))
        {
            for_all_list_liver(livers, &destroy_liver, NULL);
            for_all_deque_oper(&deque, &destroy_undo, NULL);
            destroy_list_liver(&livers);
            destroy_deque_oper(&deque);
            printf("Bad alloc\n");
            fclose(in);
            return 0;
        }
        printf("\nEnter command: ");
        scanf("%19s", command);
        type = get_command_type(command);
    }

    for_all_list_liver(livers, &destroy_liver, NULL);
    for_all_deque_oper(&deque, &destroy_undo, NULL);
    destroy_list_liver(&livers);
    destroy_deque_oper(&deque);
    return 0;
}

void parse_file(struct list_liver* list, FILE* in)
{
    struct Liver tmp;
    int status;

    do
    {
        status = parse_liver(&tmp, in);
        if (status)
        {
            status = insert_liver(list, tmp) != NULL;
        }
        if (!status)
        {
            destroy_liver(&tmp, NULL);
            for_all_list_liver(*list, &destroy_liver, NULL);
            return;
        }
        fgetc(in);
    } while (!feof(in));
}

int less_age(struct Liver lhs, struct Liver rhs)
{
    if (lhs.b.year != rhs.b.year)
        return lhs.b.year < rhs.b.year;
    else if (lhs.b.month != rhs.b.month)
        return lhs.b.month < rhs.b.month;
    else
        return lhs.b.day < rhs.b.day;
}

void destroy_liver(struct Liver* liver, void*)
{
    destroy_string(&liver->surname);
    destroy_string(&liver->name);
    destroy_string(&liver->fathername);
}

struct node_liver* insert_liver(struct list_liver* list, struct Liver liver)
{
    struct node_liver* insert_node = list->begin;
    struct node_liver* tmp_node = insert_node->next;
    while (tmp_node != NULL && less_age(liver, tmp_node->data))
    {
        insert_node = tmp_node;
        tmp_node = tmp_node->next;
    }

    struct node_liver* inserted_liver = insert_list_liver(insert_node, liver);

    return inserted_liver == NULL ? NULL : insert_node;
}

int parse_liver(struct Liver* liver, FILE* in)
{
    liver->surname = init_string_from_stream(in, &isalpha);
    liver->name = init_string_from_stream(in, &isalpha);
    liver->fathername = init_string_from_stream_no_skip(in, &isalpha);

    int counter = fscanf(in, "%zu.%zu.%zu %c %lf", &liver->b.day, &liver->b.month, &liver->b.year, &liver->sex, &liver->salary);

    if (!(is_valid_string(&liver->surname) && is_valid_string(&liver->name) && is_valid_string(&liver->fathername) &&
        !is_empty_string(&liver->surname) && !is_empty_string(&liver->name) && counter == 5 &&
        (liver->sex == 'M' || liver->sex == 'W') && liver->salary >= 0))
    {
        destroy_liver(liver, NULL);
        return 0;
    }

    return 1;
}

void print_liver(struct Liver* liver, void*)
{
    printf("%s\n%s\n%s\n%02zu.%02zu.%04zu\n%c\n%.1lf\n\n", liver->surname.data, liver->name.data, liver->fathername.data,
           liver->b.day, liver->b.month, liver->b.year, liver->sex, liver->salary);
}

enum command_type get_command_type(char* str)
{
    if (strcmp(str, "find") == 0)
    {
        return FIND;
    } else if (strcmp(str, "edit") == 0)
    {
        return EDIT;
    } else if (strcmp(str, "insert") == 0)
    {
        return INSERT;
    } else if (strcmp(str, "remove") == 0)
    {
        return REMOVE;
    } else if (strcmp(str, "print") == 0)
    {
        return PRINT;
    } else if (strcmp(str, "print_all") == 0)
    {
        return PRINT_ALL;
    } else if (strcmp(str, "undo") == 0)
    {
        return UNDO;
    } else if (strcmp(str, "quit") == 0)
    {
        return QUIT;
    } else
    {
        return UNDEF;
    }
}

enum key_type get_key_type(char* str)
{
    if (strcmp(str, "surname") == 0)
        return SURNAME;
    else if (strcmp(str, "name") == 0)
        return NAME;
    else if (strcmp(str, "fathername") == 0)
        return FATHERNAME;
    else if (strcmp(str, "date") == 0)
        return DATE;
    else if (strcmp(str, "salary") == 0)
        return SALARY;
    else
        return UNDEFINED;
}

void find(struct list_liver* list, struct deque_oper* deque)
{
    char key[20];
    printf("Enter key: ");
    scanf("%19s", key);

    enum key_type type = get_key_type(key);

    if (type == UNDEFINED)
    {
        printf("Incorrect key\n");
        return;
    }

    printf("Enter data: ");
    struct node_liver* node = NULL;
    struct Liver liver;

    switch (type)
    {
        case SURNAME:
        {
            liver.surname = init_string_from_stream(stdin, &isalpha);
            if (is_valid_string(&liver.surname))
                node = find_list_liver(*list, liver, &surname_eq);
            destroy_string(&liver.surname);
            break;
        }
        case NAME:
        {
            liver.name = init_string_from_stream(stdin, &isalpha);
            if (is_valid_string(&liver.name))
                node = find_list_liver(*list, liver, &name_eq);
            destroy_string(&liver.name);
            break;
        }
        case FATHERNAME:
        {
            liver.fathername = init_string_from_stream(stdin, &isalpha);
            if (is_valid_string(&liver.fathername))
                node = find_list_liver(*list, liver, &fathername_eq);
            destroy_string(&liver.fathername);
            break;
        }
        case DATE:
        {
            scanf("%2zu.%2zu.%4zu", &liver.b.day, &liver.b.month, &liver.b.year);
            node = find_list_liver(*list, liver, &date_eq);
            break;
        }
        case SALARY:
        {
            scanf("%lf", &liver.salary);
            node = find_list_liver(*list, liver, &salary_eq);
            break;
        }
    }

    if (node == NULL)
    {
        printf("No such liver found\n");
        return;
    }

    print_liver(&node->data, NULL);

    printf("Enter command on found item: ");
    scanf("%19s", key);
    enum command_type command = get_command_type(key);

    while (command != QUIT)
    {
        switch (command)
        {
            case EDIT:
            {
                edit(node, list, deque);
                break;
            }
            case REMOVE:
            {
                remove_(node, list, deque);
                break;
            }
            default:
            {
                printf("Wrong command\n");
            }
        }
        if (!is_list_valid_liver(*list) || !is_valid_deque_oper(deque) || command == REMOVE)
            return;
        printf("\nEnter command on found item: ");
        scanf("%19s", key);
        command = get_command_type(key);
    }
}

void insert(struct list_liver* list, struct deque_oper* deque)
{
    printf("Enter data: ");
    struct Liver liver;
    if (!parse_liver(&liver, stdin))
    {
        printf("Incorrect input\n");
        return;
    }
    struct node_liver* node = insert_liver(list, liver);

    if (node == NULL)
    {
        printf("Error while inserting\n");
        return;
    }

    struct Operation oper;
    oper.undo = &undo_insert;
    oper.delete = &delete_insert;
    oper.data.insert.id = get_id(list, node->next);

    push_operation(deque, oper);
}

int not_space(int c)
{ return !isspace(c); }

void file_print(struct Liver* liver, void* opt_data)
{
    FILE *out = opt_data;
    fprintf(out, "%s %s %s %02zu.%02zu.%04zu %c %.2lf\n", liver->surname.data, liver->name.data, liver->fathername.data,
            liver->b.day, liver->b.month, liver->b.year, liver->sex, liver->salary);
}

void print(struct list_liver* list)
{
    printf("Enter filepath: ");
    struct String path = init_string_from_stream(stdin, &not_space);

    if (!is_valid_string(&path))
    {
        printf("Bad alloc");
        return;
    }

    FILE *out = fopen(path.data, "w");

    destroy_string(&path);
    if (out == NULL)
    {
        printf("File cannot be opened\n");
        return;
    }

    for_all_list_liver(*list, &file_print, out);

    fclose(out);
}

void undo(struct deque_oper* deque, struct list_liver* list)
{
    if (!is_empty_deque_oper(deque))
    {
        --N;
        back_deque_oper(deque)->undo(&back_deque_oper(deque)->data, list);
        back_deque_oper(deque)->delete(&back_deque_oper(deque)->data);
        pop_back_deque_oper(deque);
        printf("Successfully undone\n");
    } else
        printf("Nothing to undo\n");
}

void edit(struct node_liver* liver, struct list_liver* list, struct deque_oper* deque)
{
    char key[20];
    printf("Enter key: ");
    scanf("%19s", key);
    enum key_type type = get_key_type(key);

    struct Operation oper;
    oper.undo = &undo_edit;
    oper.delete = &delete_edit;
    oper.data.edit.key = type;
    oper.data.edit.id = get_id(list, liver);

    switch (type)
    {
        case SURNAME:
        {
            printf("Enter surname: ");
            struct String str = init_string_from_stream(stdin, &isalpha);
            if (!is_valid_string(&str))
            {
                printf("Bad alloc\n");
                return;
            }
            struct String tmp = liver->data.surname;
            liver->data.surname = str;
            oper.data.edit.backup.string = tmp;
            break;
        }
        case NAME:
        {
            printf("Enter name: ");
            struct String str = init_string_from_stream(stdin, &isalpha);
            if (!is_valid_string(&str))
            {
                printf("Bad alloc\n");
                return;
            }
            struct String tmp = liver->data.name;
            liver->data.name = str;
            oper.data.edit.backup.string = tmp;
            break;
        }
        case FATHERNAME:
        {
            printf("Enter fathername: ");
            struct String str = init_string_from_stream(stdin, &isalpha);
            if (!is_valid_string(&str))
            {
                printf("Bad alloc\n");
                return;
            }
            struct String tmp = liver->data.fathername;
            liver->data.fathername = str;
            oper.data.edit.backup.string = tmp;
            break;
        }
        case DATE:
        {
            printf("Enter date: ");
            struct Date b;
            scanf("%zu.%zu.%zu", &b.day, &b.month, &b.year);
            struct Date tmp = liver->data.b;
            liver->data.b = b;
            oper.data.edit.backup.date = tmp;
            break;
        }
        case SALARY:
        {
            printf("Enter salary: ");
            double salary;
            scanf("%lf", &salary);
            double tmp = liver->data.salary;
            liver->data.salary = salary;
            oper.data.edit.backup.salary = tmp;
            break;
        }
        default:
        {
            printf("Incorrect key\n");
            return;
        }
    }

    push_operation(deque, oper);
}

void remove_(struct node_liver* liver, struct list_liver* list, struct deque_oper* deque)
{
    struct node_liver* node = list->begin;

    while (node != NULL && node->next != liver)
        node = node->next;

    if (node != NULL)
    {
        struct Liver backup = liver->data;
        remove_list_liver(node);
        struct Operation oper;
        oper.undo = &undo_remove;
        oper.delete = &delete_remove;
        oper.data.remove.backup = backup;
        push_operation(deque, oper);
    }
}

size_t get_id(struct list_liver* list, struct node_liver* node)
{
    size_t res = 0;
    struct node_liver* tmp = list->begin->next;

    while (tmp != node && tmp != NULL)
    {
        ++res;
        tmp = tmp->next;
    }
    return res;
}

int surname_eq(struct Liver lhs, struct Liver rhs)
{
    return strcmp(lhs.surname.data, rhs.surname.data) == 0;
}

int name_eq(struct Liver lhs, struct Liver rhs)
{
    return strcmp(lhs.name.data, rhs.name.data) == 0;
}

int fathername_eq(struct Liver lhs, struct Liver rhs)
{
    return strcmp(lhs.fathername.data, rhs.fathername.data) == 0;
}

int date_eq(struct Liver lhs, struct Liver rhs)
{
    return lhs.b.day == rhs.b.day && lhs.b.month == rhs.b.month &&lhs.b.year == rhs.b.year;
}

int salary_eq(struct Liver lhs, struct Liver rhs)
{
    return lhs.salary == rhs.salary;
}

void push_operation(struct deque_oper* deque, struct Operation oper)
{
    ++N;
    if (N / 2 < deque->size && !is_empty_deque_oper(deque))
    {
        destroy_undo(front_deque_oper(deque), NULL);
        pop_front_deque_oper(deque);
    }
    push_back_deque_oper(deque, oper, &destroy_undo);
}

void destroy_undo(struct Operation* oper, void*)
{
    oper->delete(&oper->data);
}

void undo_edit(union Op_data* data, struct list_liver* list)
{
    struct node_liver* node = list->begin->next;

    for (size_t i = 0; i < data->edit.id; ++i)
    {
        node = node->next;
    }

    switch (data->edit.key)
    {
        case SURNAME:
        {
            struct String tmp = node->data.surname;
            node->data.surname = data->edit.backup.string;
            data->edit.backup.string = tmp;
            break;
        }
        case NAME:
        {
            struct String tmp = node->data.name;
            node->data.name = data->edit.backup.string;
            data->edit.backup.string = tmp;
            break;
        }
        case FATHERNAME:
        {
            struct String tmp = node->data.fathername;
            node->data.fathername = data->edit.backup.string;
            data->edit.backup.string = tmp;
            break;
        }
        case DATE:
        {
            node->data.b = data->edit.backup.date;
            break;
        }
        case SALARY:
        {
            node->data.salary = data->edit.backup.salary;
            break;
        }
    }
}

void undo_insert(union Op_data* data, struct list_liver* list)
{
    struct node_liver* node = list->begin;

    for (size_t i = 0; i < data->insert.id; ++i)
    {
        node = node->next;
    }

    destroy_liver(&node->next->data, NULL);
    remove_list_liver(node);
}

void undo_remove(union Op_data* data, struct list_liver* list)
{
    insert_liver(list, data->remove.backup);
    data->remove.backup.surname.data = NULL;
    data->remove.backup.name.data = NULL;
    data->remove.backup.fathername.data = NULL;
}

void delete_edit(union Op_data* data)
{
    if (data->edit.key != DATE && data->edit.key != SALARY)
    {
        destroy_string(&data->edit.backup.string);
    }
}

void delete_insert(union Op_data*)
{

}

void delete_remove(union Op_data* data)
{
    destroy_liver(&data->remove.backup, NULL);
}