//
// Created by Des Caldnd on 11/23/2023.
//
#include "../structs.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

enum command_type
{ CREATE, REMOVE, SEARCH, RECIEVE, ALL_RECIEVED, ALL_EXPIRED, QUIT, UNDEF };

struct Address
{
    struct String city, street, building;
    int house_num, flat_num;
    char rec_ind[7];
};

struct Mail
{
    struct Address address;
    double weight;
    char post_id[15];
    char creation_time[20];
    char recieve_time[20];
    bool recieved;
};

VECTOR(Mail, struct Mail)

struct Post
{
    struct Address* post_address;
    struct vector_Mail mails;
};

void print_double(double* data, void*);
int eql_double(double lhs, double rhs)
{
    return fabs(lhs - rhs) < 0.001;
}

VECTOR(double, double)

int main()
{

}

void print_double(double* data, void*)
{
    printf("%.1lf ", *data);
}