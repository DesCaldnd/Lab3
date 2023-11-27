//
// Created by Des Caldnd on 11/27/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "../structs.h"

#define EPS 0.0001

struct Employee
{
    unsigned int id;
    struct String name;
    struct String surname;
    double salary;
};

VECTOR(Employee, struct Employee)

struct vector_Employee parse_file(FILE* in);
void print_file(struct Employee* data, void* opt);
int employee_less(const void* lhs, const void* rhs);
int employee_greater(const void* lhs, const void* rhs);
void delete_employee(struct Employee* data, void*);


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Incorrect argc\n");
        return 0;
    }

    int desc = 0;

    if (!(argv[2][0] == '-' && argv[2][2] == '\0' && (argv[2][1] == 'a' || argv[2][1] == 'd')))
    {
        printf("Incorrect flag\n");
        return 0;
    } else if (argv[2][1] == 'd')
        desc = 1;

    FILE *in = fopen(argv[1], "r");
    if (in == NULL)
    {
        printf("Input file cannot be opened\n");
        return 0;
    }
    FILE *out = fopen(argv[3], "w");

    if (out == NULL)
    {
        printf("Output file cannot be opened\n");
        fclose(in);
        return 0;
    }

    struct vector_Employee vector = parse_file(in);
    if (!is_valid_vector_Employee(&vector))
    {
        fclose(in);
        fclose(out);
        printf("Bad alloc or file is incorrect\n");
        return 0;
    }

    qsort(vector.data, vector.size, sizeof(struct Employee), desc ? &employee_less : &employee_greater);

    for_all_vector_Employee(&vector, &print_file, out);

    printf("SUCCESS");
    for_all_vector_Employee(&vector, &delete_employee, NULL);
    destroy_vector_Employee(&vector);
    fclose(in);
    fclose(out);
    return 0;
}

int parse_employee(struct Employee* emp, FILE* in)
{
    int counter = fscanf(in, "%u", &emp->id);
    emp->name = init_string_from_stream(in, &isalpha);
    emp->surname = init_string_from_stream(in, &isalpha);
    counter += fscanf(in, "%lf", &emp->salary);

    int res = 1;
    if (!is_valid_string(&emp->name) || !is_valid_string(&emp->surname) || counter != 2 || emp->name.size == 0 || emp->surname.size == 0)
    {
        destroy_string(&emp->name);
        destroy_string(&emp->surname);
        res = 0;
    }

    return res;
}

struct vector_Employee parse_file(FILE* in)
{
    struct vector_Employee vector = init_vector_Employee(10);
    if (!is_valid_vector_Employee(&vector))
        return vector;

    struct Employee tmp;

    while (!feof(in))
    {
        if (parse_employee(&tmp, in) && is_valid_vector_Employee(&vector))
            push_vector_Employee(&vector, tmp, &delete_employee);
        else
        {
            destroy_vector_Employee(&vector);
            return vector;
        }
        getc(in);
    }
    return vector;
}

void print_file(struct Employee* data, void* opt)
{
    fprintf((FILE*) opt, "%u %s %s %lf\n", data->id, data->name.data, data->surname.data, data->salary);
}

int employee_less(const void* lhs, const void* rhs)
{
    struct Employee* l = lhs, *r = rhs;
    if (fabs(r->salary - l->salary) > EPS)
        return l->salary < r->salary;
    int stat = strcmp(l->surname.data, r->surname.data);
    if (stat != 0)
        return stat == -1;
    stat = strcmp(l->name.data, r->name.data);
    if (stat != 0)
        return stat == -1;

    return l->id < r->id;
}

int employee_greater(const void* lhs, const void* rhs)
{
    struct Employee* l = lhs, *r = rhs;
    if (fabs(r->salary - l->salary) > EPS)
        return l->salary > r->salary;
    int stat = strcmp(l->surname.data, r->surname.data);
    if (stat != 0)
        return stat == 1;
    stat = strcmp(l->name.data, r->name.data);
    if (stat != 0)
        return stat == 1;

    return l->id > r->id;
}

void delete_employee(struct Employee* data, void*)
{
    destroy_string(&data->name);
    destroy_string(&data->surname);
}