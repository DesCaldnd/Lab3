//
// Created by Des Caldnd on 11/29/2023.
//
#include "../structs.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

enum command_type
{ FIND, SORT, UPPER, PRINT_ALL, QUIT, UNDEF };

enum key_type
{ ID, SURNAME, NAME, GROUP, UNDEFINED };

VECTOR(uchar, unsigned char)

struct av_data
{
    unsigned int count, size;
};

struct print_data
{
    struct av_data data;
    FILE* out;
};

struct Student
{
    unsigned int id;
    struct String name;
    struct String surname;
    struct String group;
    struct vector_uchar marks;
};

VECTOR(student, struct Student)

void parse_file(struct vector_student *vector, FILE* in);
int parse_student(struct Student* student, FILE* in);
enum command_type get_command_type(char* str);
enum key_type get_key_type(char* str);
int not_sep(int c) { return !isspace(c); }
void destroy_student(struct Student* st, void*);
void destroy_uchar(unsigned char*, void*){}
void find(struct vector_student* vector, FILE* out);
void sort(struct vector_student* vector, FILE* out);
void upper(struct vector_student* vector, FILE* out);

int id_cmp(const void* lhs, const void* rhs);
int surname_cmp(const void* lhs, const void* rhs);
int name_cmp(const void* lhs, const void* rhs);
int group_cmp(const void* lhs, const void* rhs);

int id_eq(struct Student lhs, struct Student rhs);
int surname_eq(struct Student lhs, struct Student rhs);
int name_eq(struct Student lhs, struct Student rhs);
int group_eq(struct Student lhs, struct Student rhs);

void average(struct Student* stud, void* opt_data);
struct av_data calculate_average(struct Student stud);

void print_student(struct Student* stud, void* opt_data);
void print_student_av(struct Student* stud, void* opt_data);

void print_to(FILE* out, const char* format, ...);

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Incorrect argc\n");
        return 0;
    }

    FILE *in = fopen(argv[1], "r");

    if (in == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    FILE *out = fopen(argv[2], "w");

    if (out == NULL)
    {
        printf("Unable to open output file\n");
        fclose(in);
        return 0;
    }

    struct vector_student vector = init_vector_student(10);

    if (!is_valid_vector_student(&vector))
    {
        printf("Bad alloc\n");
        fclose(in);
        fclose(out);
        return 0;
    }

    parse_file(&vector, in);

    if (!is_valid_vector_student(&vector))
    {
        printf("Bad alloc or incorrect data\n");
        fclose(in);
        fclose(out);
        return 0;
    }

    char prompt[20];
    printf("Enter command: ");
    scanf("%19s", prompt);
    enum command_type command = get_command_type(prompt);

    while(command != QUIT)
    {
        switch (command)
        {
            case FIND:
            {
                find(&vector, out);
                break;
            }
            case SORT:
            {
                sort(&vector, out);
                break;
            }
            case UPPER:
            {
                upper(&vector, out);
                break;
            }
            case PRINT_ALL:
            {
                print_to(out, "Print_all command output:\n");
                for_all_vector_student(&vector, &print_student, out);
                break;
            }
            default:
            {
                printf("Incorrect command. You should use one of this:\n\t-sort\n\t-find\n\t-upper\n\t-quit\n");
                break;
            }
        }

        if(!is_valid_vector_student(&vector))
        {
            print_to(out, "Bad alloc\n");
            fclose(in);
            fclose(out);
            return 0;
        }
        print_to(out, "\n");
        printf("Enter command: ");
        scanf("%19s", prompt);
        command = get_command_type(prompt);
    }

    for_all_vector_student(&vector, &destroy_student, NULL);
    destroy_vector_student(&vector);
    fclose(in);
    fclose(out);
    return 0;
}

void parse_file(struct vector_student *vector, FILE* in)
{
    struct Student tmp;
    int status;

    while (!feof(in))
    {
        status = parse_student(&tmp, in);
        if (!status)
        {
            destroy_vector_student(vector);
            return;
        }

        push_vector_student(vector, tmp, &destroy_student);
        if (!is_valid_vector_student(vector))
            return;
        getc(in);
    }
}

int parse_student(struct Student* student, FILE* in)
{
    int counter = 0;
    counter += fscanf(in, "%u", &student->id);

    student->name = init_string_from_stream(in, &isalpha);
    student->surname = init_string_from_stream(in, &isalpha);
    student->group = init_string_from_stream(in, &not_sep);

    student->marks = init_vector_uchar(5);

    if(is_valid_vector_uchar(&student->marks))
    {
        for (int i = 0; i < 5; ++i)
        {
            unsigned char c;
            char d;
            counter += fscanf(in, "%c", &d);
            c = d;
            push_vector_uchar(&student->marks, c, &destroy_uchar);
        }
    }
    if (!(is_valid_string(&student->name) && is_valid_string(&student->surname) && is_valid_string(&student->group) &&
        !is_empty_string(&student->name) && !is_empty_string(&student->surname) && !is_empty_string(&student->group) &&
            is_valid_vector_uchar(&student->marks) && counter == 6))
    {
        destroy_vector_uchar(&student->marks);
        destroy_string(&student->name);
        destroy_string(&student->surname);
        destroy_string(&student->group);
        return 0;
    }
    return 1;
}

enum command_type get_command_type(char* str)
{
    if (strcmp(str, "find") == 0)
    {
        return FIND;
    } else if (strcmp(str, "sort") == 0)
    {
        return SORT;
    } else if (strcmp(str, "upper") == 0)
    {
        return UPPER;
    } else if (strcmp(str, "print_all") == 0)
    {
        return PRINT_ALL;
    }
    else if (strcmp(str, "quit") == 0)
    {
        return QUIT;
    } else
    {
        return UNDEF;
    }
}

enum key_type get_key_type(char* str)
{
    if (strcmp(str, "id") == 0)
        return ID;
    else if (strcmp(str, "surname") == 0)
        return SURNAME;
    else if (strcmp(str, "name") == 0)
        return NAME;
    else if (strcmp(str, "group") == 0)
        return GROUP;
    else
        return UNDEFINED;
}

void destroy_student(struct Student* st, void*)
{
    destroy_string(&st->name);
    destroy_string(&st->surname);
    destroy_string(&st->group);
    destroy_vector_uchar(&st->marks);
}

void find(struct vector_student* vector, FILE* out)
{
    struct Student* stud, data;
    char key[20];
    printf("Enter key: ");
    scanf("%s", key);

    enum key_type type = get_key_type(key);

    switch (type)
    {
        case ID:
        {
            printf("Enter id: ");
            int counter = scanf("%u", &data.id);

            if (counter != 1)
            {
                print_to(out, "Incorrect input\n");
                return;
            }

            stud = find_vector_student(vector, data, &id_eq);

            break;
        }
        case SURNAME:
        {
            printf("Enter surname: ");
            data.surname = init_string_from_stream(stdin, &isalpha);

            if (!is_valid_string(&data.surname))
            {
                print_to(out, "Bad alloc\n");
                return;
            }

            stud = find_vector_student(vector, data, &surname_eq);
            destroy_string(&data.surname);
            break;
        }
        case NAME:
        {
            printf("Enter surname: ");
            data.name = init_string_from_stream(stdin, &isalpha);

            if (!is_valid_string(&data.name))
            {
                print_to(out, "Bad alloc\n");
                return;
            }

            stud = find_vector_student(vector, data, &name_eq);
            destroy_string(&data.name);
            break;
        }
        case GROUP:
        {
            printf("Enter surname: ");
            data.group = init_string_from_stream(stdin, &not_sep);

            if (!is_valid_string(&data.group))
            {
                print_to(out, "Bad alloc\n");
                return;
            }

            stud = find_vector_student(vector, data, &group_eq);
            destroy_string(&data.group);
            break;
        }
        default:
        {
            printf("Incorrect key\n");
            return;
        }
    }

    if (stud != NULL)
    {
        print_to(out, "Find command with key %s result:\n", key);
        print_student(stud, out);
    } else
    {
        print_to(out, "No student found\n");
    }
}

void sort(struct vector_student* vector, FILE* out)
{
    char key[20];
    printf("Enter key: ");
    scanf("%s", key);

    enum key_type type = get_key_type(key);

    switch (type)
    {
        case ID:
        {
            qsort(vector->data, vector->size, sizeof(struct Student), &id_cmp);
            break;
        }
        case SURNAME:
        {
            qsort(vector->data, vector->size, sizeof(struct Student), &surname_cmp);
            break;
        }
        case NAME:
        {
            qsort(vector->data, vector->size, sizeof(struct Student), &name_cmp);
            break;
        }
        case GROUP:
        {
            qsort(vector->data, vector->size, sizeof(struct Student), &group_cmp);
            break;
        }
        default:
        {
            print_to(out, "Incorrect key\n");
            return;
            break;
        }
    }

    print_to(out, "Successfully sorted by %s\n", key);
}

void upper(struct vector_student* vector, FILE* out)
{
    struct av_data av;
    av.size = av.count = 0;
    for_all_vector_student(vector, &average, &av);

    print_to(out, "Upper command output:\n");
    struct print_data pr;
    pr.data = av;
    pr.out = out;

    for_all_vector_student(vector, &print_student_av, &pr);
}

int id_cmp(const void* lhs, const void* rhs)
{
    unsigned char l = ((const struct Student*)lhs)->id, r = ((const struct Student*)rhs)->id;
    if (l < r)
        return -1;
    else
        return l > r;
}

int surname_cmp(const void* lhs, const void* rhs)
{
    return strcmp(((const struct Student*)lhs)->surname.data, ((const struct Student*)rhs)->surname.data);
}

int name_cmp(const void* lhs, const void* rhs)
{
    return strcmp(((const struct Student*)lhs)->name.data, ((const struct Student*)rhs)->name.data);
}

int group_cmp(const void* lhs, const void* rhs)
{
    return strcmp(((const struct Student*)lhs)->group.data, ((const struct Student*)rhs)->group.data);
}

int id_eq(struct Student lhs, struct Student rhs)
{
    return lhs.id == rhs.id;
}

int surname_eq(struct Student lhs, struct Student rhs)
{
    return strcmp(lhs.surname.data, rhs.surname.data) == 0;
}

int name_eq(struct Student lhs, struct Student rhs)
{
    return strcmp(lhs.name.data, rhs.name.data) == 0;
}

int group_eq(struct Student lhs, struct Student rhs)
{
    return strcmp(lhs.group.data, rhs.group.data) == 0;
}

void average(struct Student* stud, void* opt_data)
{
    struct av_data* data = opt_data, current = calculate_average(*stud);
    data->size += current.size;
    data->count += current.count;
}

struct av_data calculate_average(struct Student stud)
{
    struct av_data res;
    res.size = stud.marks.size;
    res.count = 0;

    for (int i = 0; i < res.size; ++i)
    {
        res.count += stud.marks.data[i];
    }
    return res;
}

void print_student(struct Student* stud, void* opt_data)
{
    FILE *out = opt_data;
    print_to(out, "%u %s %s %s\n", stud->id, stud->name.data, stud->surname.data, stud->group.data);
}

void print_student_av(struct Student* stud, void* opt_data)
{
    struct print_data *p_data = opt_data;
    struct av_data av = calculate_average(*stud);
    double cur = ((double)av.count) / av.size, glob = ((double)p_data->data.count) / p_data->data.size;

    if (cur > glob)
    {
        print_to(p_data->out, "%u %s %s %s %.2lf\n", stud->id, stud->name.data, stud->surname.data, stud->group.data, cur);
    }
}

void print_to(FILE* out, const char* format, ...)
{
    va_list arg1, arg2;
    va_start(arg1, format);
    va_copy(arg2, arg1);
    vprintf(format, arg1);
    vfprintf(out, format, arg2);
}