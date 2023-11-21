//
// Created by Des Caldnd on 11/21/2023.
//
#include "structs.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

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

int cmpf_insert_buses(struct list_bus lhs, struct list_bus rhs);
int less_time(struct bus lhs, struct bus rhs);

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

    char qual[4], comm[20];
    enum command_type q_type, c_type;
    do
    {
        scanf("%3s %19s", qual, comm);
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
            while (less_time(data, tmp_node->data) && tmp_node != NULL)
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

}

void do_min(struct list_list_bus list, enum command_type c_type)
{

}

void free_list(struct list_bus* list, void*)
{
    destroy_list_bus(list);
}

int cmpf_insert_buses(struct list_bus lhs, struct list_bus rhs)
{
    return strncmp(lhs.begin->data.id, rhs.begin->data.id, 7) == -1;
}

int less_time(struct bus lhs, struct bus rhs)
{
    time_t l = mktime(&lhs.st_time), r = mktime(&rhs.st_time);
    return difftime(l, r) < 0;
}