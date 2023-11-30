//
// Created by Des Caldnd on 11/21/2023.
//

#ifndef LAB3_STRUCTS_H
#define LAB3_STRUCTS_H

#include <stdlib.h>
#include <stdio.h>

#define LIST(type, st_type) struct node_##type   \
{                                      \
    st_type data;                         \
    struct node_##type* next;           \
};                                      \
                                        \
struct list_##type                      \
{                                       \
    struct node_##type* begin;                                               \
};                                                                          \
                                                                            \
struct list_##type init_list_##type()                                            \
{                                                                           \
    struct list_##type res;                                                 \
    res.begin = malloc(sizeof(struct node_##type));                         \
    if (res.begin != NULL)                                                  \
    {                                                                       \
        res.begin->next = NULL;                                              \
    }                                                                       \
    return res;                                                             \
}                                       \
                                        \
int is_list_valid_##type(struct list_##type list)                                \
{                                       \
    return list.begin != NULL;                                         \
}                                       \
                                        \
int is_list_empty_##type(struct list_##type list)                           \
{                                       \
    return list.begin->next == NULL;                                        \
}                                       \
                                        \
struct node_##type* insert_list_##type(struct node_##type *node, st_type data)              \
{                                                \
    struct node_##type *tmp = malloc(sizeof(struct node_##type));           \
    if (tmp == NULL)                             \
        return NULL;                                \
                                                 \
    tmp->data = data;                            \
    tmp->next = node->next;                      \
    node->next = tmp;                            \
    return tmp;\
}                                                \
                                                 \
void destroy_list_##type(struct list_##type *list)                          \
{                                                \
    struct node_##type *node = list->begin;       \
    while (node != NULL)                         \
    {                                            \
        struct node_##type* tmp = node->next;      \
        free(node);                              \
        node = tmp;\
    }                                            \
    list->begin = NULL;                                                 \
}                                                \
                                                 \
int size_list_##type(struct list_##type list)    \
{                                                \
    int res = 0;                                 \
    struct node_##type* node = list.begin->next; \
    while (node != NULL)                         \
    {                                            \
        ++res;                                   \
        node = node->next;\
    }                                            \
    return res;\
}                                                \
                                                 \
struct node_##type* find_list_##type(struct list_##type list, st_type data, int (*cmpf)(st_type, st_type))              \
{                                                \
    struct node_##type* node = list.begin->next; \
    while(node != NULL && (!cmpf(node->data, data)))    \
    {                                            \
        node = node->next;                                             \
    }                                            \
    return node;\
}                                                \
                                                 \
void for_all_list_##type(struct list_##type list, void (*func)(st_type*, void*), void* opt_data)    \
{                                                \
    struct node_##type* node = list.begin->next;       \
    while (node != NULL)                         \
    {                                            \
        func(&node->data, opt_data);                       \
        node = node->next;\
    }\
}\

//---------------------------------------------------------

#define VECTOR(type, st_type) struct vector_##type\
{\
    st_type *data;\
    size_t size, capacity;\
};\
\
struct vector_##type init_vector_##type(size_t size)\
{\
    struct vector_##type vector;\
    vector.data = malloc(sizeof(st_type) * size);\
    vector.size = 0;\
    vector.capacity = size;\
    return vector;\
}\
\
int is_empty_vector_##type(struct vector_##type* vector)\
{\
    return vector->size <= 0;\
}\
\
int is_valid_vector_##type(struct vector_##type* vector)\
{\
    return vector->data != NULL;\
}                                                 \
\
void for_all_vector_##type(struct vector_##type* vector, void (*func)(st_type*, void*), void* opt_data)\
{\
    for (int i = 0; i < vector->size; ++i)\
    {\
        func(&vector->data[i], opt_data);\
    }\
}                                                 \
\
void realloc_vector_##type(struct vector_##type* vector, size_t size, void (*deleter)(st_type*, void*))\
{                                                 \
    st_type* tmp = malloc(sizeof(st_type) * size);\
    if (tmp == NULL)                              \
    {                                             \
        for_all_vector_##type(vector, deleter, NULL); \
        free(vector->data);                       \
        vector->size = 0;                         \
        vector->capacity = 0;                     \
        vector->data = NULL;\
    } else                                        \
    {\
        if (vector->size > size)\
            vector->size = size;\
        vector->capacity = size;                  \
        for (size_t i = 0; i < vector->size; ++i)    \
        {                                         \
            tmp[i] = vector->data[i];                                          \
        }                                         \
        free(vector->data);                       \
        vector->data = tmp;\
    }\
}\
\
void push_vector_##type(struct vector_##type* vector, st_type data, void (*deleter)(st_type*, void*))\
{\
    if (vector->size >= vector->capacity)\
        realloc_vector_##type(vector, vector->capacity * 2, deleter);\
\
    if (is_valid_vector_##type(vector))\
    {\
        vector->data[vector->size] = data;\
        ++vector->size;\
    }\
}\
\
void pop_vector_##type(struct vector_##type* vector)\
{\
    if (vector->size > 0)\
        --vector->size;\
}\
st_type top_vector_##type(struct vector_##type* vector)\
{\
    return vector->data[vector->size - 1];\
}\
void destroy_vector_##type(struct vector_##type* vector)\
{\
    if (vector->data != NULL)\
        free(vector->data);\
    vector->size = vector->capacity = 0;            \
    vector->data = NULL;\
}\
\
void insert_vector_##type(struct vector_##type* vector, st_type data, size_t ind, void (*deleter)(st_type*, void*))\
{\
    if (vector->size >= vector->capacity)\
        realloc_vector_##type(vector, vector->capacity * 2, deleter);\
\
    if (is_valid_vector_##type(vector))\
    {\
        if (ind > vector->size)\
            ind = vector->size;\
        st_type tmp;\
        for (int i = ind; i < vector->size + 1; ++i)\
        {\
            tmp = vector->data[i];\
            vector->data[i] = data;\
            data = tmp;\
        }\
        ++vector->size;\
    }\
}\
\
void remove_vector_##type(struct vector_##type* vector, size_t ind)\
{\
    if (ind > vector->size)\
        ind = vector->size;\
\
    for (int i = ind; i < vector->size - 1; ++i)\
    {\
        vector->data[i] = vector->data[i + 1];\
    }\
    --vector->size;\
}\
\
st_type* find_vector_##type(struct vector_##type* vector, st_type data, int (*comp)(st_type, st_type))\
{\
    st_type* res = NULL;\
    for (int i = 0; i < vector->size; ++i)\
    {\
        if (comp(vector->data[i], data))\
        {\
            res = &vector->data[i];\
            break;\
        }\
    }\
    return res;\
}\
\
size_t size_vector_##type(struct vector_##type* vector)\
{\
    return vector->size;\
}


//---------------------------------------------------------

struct String
{
    char *data;
    int size, capacity;
};

struct String init_string(const char *src);

struct String init_string_size(size_t size);

struct String init_string_from_stream(FILE* stream, int (*is_needed_sym)(int));

void destroy_string(struct String *data);

int is_valid_string(struct String *data);

int is_empty_string(struct String *data);

void realloc_string(struct String *data, int size);

void push_string_c(struct String *data, char c);

void cat_string(struct String *dest, const struct String *src);

int cmp_length_string(struct String *lhs, struct String *rhs);

int cmp_string(struct String *lhs, struct String *rhs);

void cpy_string(struct String *dest, const struct String *src);

struct String *cpy_dyn_string(const struct String *src);

#endif //LAB3_STRUCTS_H
