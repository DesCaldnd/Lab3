//
// Created by Des Caldnd on 11/21/2023.
//
#include "../structs.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

enum stop_type
{ START, PASS, FINISH };

enum error_type
{ CORRECT, ERROR };

enum command_type
{ MAX, MIN, ROUTE_COUNT, PATH_LENGTH, ROUTE_LENGTH, STOP_LENGTH, STAY_LENGTH, QUIT, UNDEF };

struct bus
{
    char id[7];
    double station_x, station_y;
    enum stop_type type;
    struct tm st_time, end_time;
};

LIST(bus, struct bus)
LIST(list_bus, struct list_bus)

void parse_file(struct list_list_bus* list, FILE* in);
enum command_type get_command_type(char *str);
void do_max(struct list_list_bus list, enum command_type c_type);
void do_min(struct list_list_bus list, enum command_type c_type);

void free_list(struct list_bus* list, void*);

void find_path_length(struct list_list_bus list, int is_max);
void find_route_count(struct list_list_bus list, int is_max);
void find_route_length(struct list_list_bus list, int is_max);
void find_stop_length(struct list_list_bus list, int is_max);
void find_stay_length(struct list_list_bus list, int is_max);

int cmpf_insert_buses(struct list_bus lhs, struct list_bus rhs);
int less_time(struct bus lhs, struct bus rhs);

void print_l_list(struct list_bus *list, void*);
void print_list(struct bus *b, void*);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Incorrect argument count. You must pass list of log filepaths\n");
        return 0;
    }
    struct list_list_bus gen_list = init_list_list_bus();
    if (!is_list_valid_list_bus(gen_list))
    {
        printf("Bad alloc\n");
        return 0;
    }

    for (int i = 1; i < argc; ++i)
    {
        FILE* in = fopen(argv[i], "r");
        if (in != NULL)
        {
            parse_file(&gen_list, in);
            fclose(in);
            if (!is_list_valid_list_bus(gen_list))
            {
                printf("Bad alloc\n");
                return 0;
            }
        } else
        {
            printf("File \"%s\" cannot be opened\n", argv[i]);
        }
    }

//    for_all_list_list_bus(gen_list, &print_l_list, NULL);

    char qual[5], comm[20];
    enum command_type q_type, c_type;
    do
    {
        scanf("%4s %19s", qual, comm);
        q_type = get_command_type(qual);
        c_type = get_command_type(comm);
        if (q_type != UNDEF && q_type != QUIT && (q_type == MAX || q_type == MIN) && c_type != UNDEF && c_type != QUIT && c_type != MAX && c_type != MIN)
        {
            if(q_type == MAX)
                do_max(gen_list, c_type);
            else
                do_min(gen_list, c_type);
        } else if (!(c_type == QUIT || q_type == QUIT))
            printf("Wrong command\n");
    } while (c_type != QUIT && q_type != QUIT);

    for_all_list_list_bus(gen_list, &free_list, NULL);
    destroy_list_list_bus(&gen_list);

    return 0;
}

void parse_file(struct list_list_bus* list, FILE* in)
{
    double x, y;
    fscanf(in, "%lf %lf", &x, &y);

    while(!feof(in))
    {
        char id[7];
        fscanf(in, "%6s", id);
        unsigned int year, month, day, hour, minute, second;
        struct tm st, fn;
        fscanf(in, "%2u.%2u.%4u %2u:%2u:%2u", &day, &month, &year, &hour, &minute, &second);
        st.tm_sec = second;
        st.tm_min = minute;
        st.tm_hour = hour;
        st.tm_isdst = 0;
        st.tm_year = year - 1900;
        st.tm_mon = month - 1;
        st.tm_mday = day;
        fscanf(in, "%2u.%2u.%4u %2u:%2u:%2u", &day, &month, &year, &hour, &minute, &second);
        fn.tm_sec = second;
        fn.tm_min = minute;
        fn.tm_hour = hour;
        fn.tm_isdst = 0;
        fn.tm_year = year - 1900;
        fn.tm_mon = month - 1;
        fn.tm_mday = day;
        char c;
        fscanf(in, " %c", &c);
        enum stop_type type = c == 'S' ? START : c == 'P' ? PASS : FINISH;
        struct bus data;

        strncpy(data.id, id, 6);
        data.station_x = x;
        data.station_y = y;
        data.st_time = st;
        data.end_time = fn;
        data.type = type;

        struct node_bus a, b;
        a.next = &b;
        b.next = NULL;
        b.data = data;
        struct list_bus tmp_list;
        tmp_list.begin = &a;
        struct node_list_bus* in_node = find_list_list_bus(*list, tmp_list, &cmpf_insert_buses);

        if (in_node == NULL)
        {
            struct list_bus bucket = init_list_bus();
            if (!is_list_valid_bus(bucket))
            {
                for_all_list_list_bus(*list, &free_list, NULL);
                destroy_list_list_bus(list);
                return;
            }
            in_node = insert_list_list_bus(list->begin, bucket);
            if (in_node == NULL)
            {
                destroy_list_bus(&bucket);
                for_all_list_list_bus(*list, &free_list, NULL);
                destroy_list_list_bus(list);
                return;
            }
        }

        struct node_bus* insert_node = in_node->data.begin;
        if(!is_list_empty_bus(in_node->data))
        {
            struct node_bus* tmp_node = insert_node->next;
            while (tmp_node != NULL && less_time(tmp_node->data, data))
            {
                insert_node = tmp_node;
                tmp_node = tmp_node->next;
            }
        }

        struct node_bus* inserted_bus = insert_list_bus(insert_node, data);
        if (inserted_bus == NULL)
        {
            for_all_list_list_bus(*list, &free_list, NULL);
            destroy_list_list_bus(list);
        }
        fgetc(in);
    }
}

enum command_type get_command_type(char *str)
{
    if (strcmp(str, "max") == 0)
        return MAX;
    else if (strcmp(str, "min") == 0)
        return MIN;
    else if (strcmp(str, "route_count") == 0)
        return ROUTE_COUNT;
    else if (strcmp(str, "path_length") == 0)
        return PATH_LENGTH;
    else if (strcmp(str, "route_length") == 0)
        return ROUTE_LENGTH;
    else if (strcmp(str, "stop_length") == 0)
        return STOP_LENGTH;
    else if (strcmp(str, "stay_length") == 0)
        return STAY_LENGTH;
    else if (strcmp(str, "quit") == 0)
        return QUIT;
    else
        return UNDEF;
}

void do_max(struct list_list_bus list, enum command_type c_type)
{
    switch (c_type)
    {
        case ROUTE_COUNT:
        {
            find_route_count(list, 1);
            break;
        }
        case PATH_LENGTH:
        {
            find_path_length(list, 1);
            break;
        }
        case ROUTE_LENGTH:
        {
            find_route_length(list, 1);
            break;
        }
        case STOP_LENGTH:
        {
            find_stop_length(list, 1);
            break;
        }
        case STAY_LENGTH:
        {
            find_stay_length(list, 1);
            break;
        }
    }
}

void do_min(struct list_list_bus list, enum command_type c_type)
{
    switch (c_type)
    {
        case ROUTE_COUNT:
        {
            find_route_count(list, 0);
            break;
        }
        case PATH_LENGTH:
        {
            find_path_length(list, 0);
            break;
        }
        case ROUTE_LENGTH:
        {
            find_route_length(list, 0);
            break;
        }
        case STOP_LENGTH:
        {
            find_stop_length(list, 0);
            break;
        }
        case STAY_LENGTH:
        {
            find_stay_length(list, 0);
            break;
        }
    }
}

void free_list(struct list_bus* list, void*)
{
    destroy_list_bus(list);
}

int cmpf_insert_buses(struct list_bus lhs, struct list_bus rhs)
{
    return strncmp(lhs.begin->next->data.id, rhs.begin->next->data.id, 6) == 0;
}

int less_time(struct bus lhs, struct bus rhs)
{
    time_t l = mktime(&lhs.st_time), r = mktime(&rhs.st_time);
    return difftime(l, r) < 0;
}

void print_l_list(struct list_bus *list, void*)
{
    for_all_list_bus(*list, &print_list, NULL);
    printf("\n");
}

void print_list(struct bus *b, void*)
{
    printf("%6s ", b->id);
}

double path_length(struct list_bus list)
{
    struct node_bus* node = list.begin->next;
    double x = node->data.station_x, y = node->data.station_y, res = 0;
    node = node->next;
    while (node != NULL)
    {
        res += sqrt((x-node->data.station_x) * (x-node->data.station_x) + (y-node->data.station_y) * (y-node->data.station_y));
        x = node->data.station_x;
        y = node->data.station_y;
        node = node->next;
    }
    return res;
}

void find_path_length(struct list_list_bus list, int is_max)
{
    struct node_list_bus* node = list.begin->next, *res_n = node;
    double res = path_length(node->data);
    node = node->next;

    while (node != NULL)
    {
        double tmp = path_length(node->data);
        if ((is_max && tmp > res) || (!is_max && tmp < res))
        {
            res = tmp;
            res_n = node;
        }
        node = node->next;
    }
    printf("%s path length = %lf at %6s bus\n", is_max ? "Maximum" : "Minimum", res, res_n->data.begin->next->data.id);
}

int route_count(struct list_bus list)
{
    struct node_bus* node = list.begin->next;
    int res = 0, started = 0;
    while (node != NULL)
    {
        if (node->data.type == START)
            started = 1;
        else if (node->data.type == FINISH && started)
        {
            started = 0;
            ++res;
        }
        node = node->next;
    }
    return res;
}

void find_route_count(struct list_list_bus list, int is_max)
{
    struct node_list_bus* node = list.begin->next, *res_n = node;
    int res = route_count(node->data);
    node = node->next;

    while (node != NULL)
    {
        int tmp = route_count(node->data);
        if ((is_max && tmp > res) || (!is_max && tmp < res))
        {
            res = tmp;
            res_n = node;
        }
        node = node->next;
    }
    printf("%s route count = %d at %6s bus\n", is_max ? "Maximum" : "Minimum", res, res_n->data.begin->next->data.id);
}

double route_length(struct list_bus list, int is_max)
{
    struct node_bus* node = list.begin->next;
    double x = node->data.station_x, y = node->data.station_y, len = 0, res = 0;
    int calculated = 0, started = 1;
    node = node->next;
    while (node != NULL)
    {
        if (started)
            len += sqrt((x-node->data.station_x) * (x-node->data.station_x) + (y-node->data.station_y) * (y-node->data.station_y));
        if (node->data.type == START)
            started = 1;
        else if (node->data.type == FINISH)
        {
            if ((is_max && len > res) || (!is_max && len < res) || !calculated)
            {
                calculated = 1;
                res = len;
            }
            len = 0;
            started = 0;
        }
        x = node->data.station_x;
        y = node->data.station_y;
        node = node->next;
    }
    return res;
}

void find_route_length(struct list_list_bus list, int is_max)
{
    struct node_list_bus* node = list.begin->next, *res_n = node;
    double res = route_length(node->data, is_max);
    node = node->next;

    while (node != NULL)
    {
        double tmp = route_length(node->data, is_max);
        if ((is_max && tmp > res) || (!is_max && tmp < res))
        {
            res = tmp;
            res_n = node;
        }
        node = node->next;
    }
    printf("%s route length = %lf at %6s bus\n", is_max ? "Maximum" : "Minimum", res, res_n->data.begin->next->data.id);
}

double stop_length(struct list_bus list, int is_max)
{
    struct node_bus* node = list.begin->next;
    time_t st = mktime(&node->data.st_time), end = mktime(&node->data.end_time);
    double res = difftime(end, st);
    node = node->next;
    while (node != NULL)
    {
        st = mktime(&node->data.st_time), end = mktime(&node->data.end_time);
        double tmp = difftime(end, st);
        if ((is_max && tmp > res) || (!is_max && tmp < res))
            res = tmp;
        node = node->next;
    }
    return res;
}

void find_stop_length(struct list_list_bus list, int is_max)
{
    struct node_list_bus* node = list.begin->next, *res_n = node;
    double res = stop_length(node->data, is_max);
    node = node->next;

    while (node != NULL)
    {
        double tmp = stop_length(node->data, is_max);
        if ((is_max && tmp > res) || (!is_max && tmp < res))
        {
            res = tmp;
            res_n = node;
        }
        node = node->next;
    }
    printf("%s stop length = %.1lf sec at %6s bus\n", is_max ? "Maximum" : "Minimum", res, res_n->data.begin->next->data.id);
}

double stay_length(struct list_bus list)
{
    struct node_bus* node = list.begin->next;
    time_t st, end;
    double res = 0;
    while (node != NULL)
    {
        st = mktime(&node->data.st_time), end = mktime(&node->data.end_time);
        res += difftime(end, st);
        node = node->next;
    }
    return res;
}

void find_stay_length(struct list_list_bus list, int is_max)
{
    struct node_list_bus* node = list.begin->next, *res_n = node;
    double res = stay_length(node->data);
    node = node->next;

    while (node != NULL)
    {
        double tmp = stay_length(node->data);
        if ((is_max && tmp > res) || (!is_max && tmp < res))
        {
            res = tmp;
            res_n = node;
        }
        node = node->next;
    }
    printf("%s stay length = %.1lf sec at %6s bus\n", is_max ? "Maximum" : "Minimum", res, res_n->data.begin->next->data.id);
}