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
void remove_list_##type(struct node_##type *node)             \
{                                                \
    if (node->next == NULL)                      \
        return;\
    struct node_##type* tmp = node->next;         \
    node->next = node->next->next;               \
    free(tmp);\
}\
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
struct list_##type copy_list_##type(const struct list_##type src)            \
{                                                \
    struct list_##type result = init_list_##type();                          \
    if (!is_list_valid_##type(result))           \
        return result;                           \
                                                 \
    struct node_##type* it = src.begin->next, *inserter = result.begin;    \
                                                 \
    while (it != NULL)                           \
    {                                            \
        inserter = insert_list_##type(inserter, it->data);                   \
                                                 \
        if (inserter == NULL)                    \
        {                                        \
            destroy_list_##type(&result);        \
            return result;\
        }\
                                                 \
        it = it->next;\
    }                                            \
                                                 \
    return result;\
}\
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

struct String init_string_from_stream_buf(FILE* stream, char* buf, int (*is_needed_sym)(int), int (*no_skip_sym)(int));

struct String init_string_from_stream_no_skip(FILE* stream, int (*is_needed_sym)(int));

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

//----------------------------------------------------------------------------

#define DEQUE(type, st_type) struct deque_##type\
{\
    st_type *data;\
    size_t size, capacity, start;\
};\
\
struct deque_##type init_deque_##type(size_t size)\
{\
    struct deque_##type deque;\
    if (size == 0)\
        size = 1;\
    deque.data = malloc(size * sizeof(st_type));\
    deque.size = 0;\
    deque.start = 0;\
    deque.capacity = deque.data == NULL ? 0 : size;\
    return deque;\
}\
\
st_type *get_deque_##type(struct deque_##type* deque, size_t index)\
{\
    return &deque->data[(index + deque->start) % deque->capacity];\
}\
\
void for_all_deque_##type(struct deque_##type* deque, void (*func)(st_type*, void*), void* opt_data)\
{\
    for (size_t i = 0; i < deque->size; ++i)\
    {\
        func(get_deque_##type(deque, i), opt_data);\
    }\
}\
\
void destroy_deque_##type(struct deque_##type* deque)\
{\
    if (deque->data != NULL)\
    {\
        free(deque->data);\
        deque->data = NULL;\
    }\
    deque->start = 0;\
    deque->size = 0;\
    deque->capacity = 0;\
}\
\
void realloc_deque_##type(struct deque_##type* deque, size_t size, void (*deleter)(st_type*, void*))\
{\
    st_type * tmp = malloc(sizeof(st_type) * size);\
    if (tmp == NULL)\
    {\
        for_all_deque_##type(deque, deleter, NULL);\
        destroy_deque_##type(deque);\
    } else\
    {\
        for (size_t i = 0; i < deque->size && i < size; ++i)\
        {\
            tmp[i] = *get_deque_##type(deque, i);\
        }\
\
        if (deque->size > size)\
        {\
            deque->size = size;\
            for (size_t i = deque->size; i < size; ++i)\
            {\
                deleter(get_deque_##type(deque, i), NULL);\
            }\
        }\
        deque->capacity = size;\
        free(deque->data);\
        deque->data = tmp;\
        deque->start = 0;\
    }\
}\
\
int is_valid_deque_##type(struct deque_##type* deque)\
{\
    return deque->data != NULL;\
}\
\
int is_empty_deque_##type(struct deque_##type* deque)\
{\
    return deque->size == 0;\
}\
\
int push_front_deque_##type(struct deque_##type* deque, st_type data, void (*deleter)(st_type*, void*))\
{\
    if (deque->capacity <= deque->size + 1)\
    {\
        realloc_deque_##type(deque, deque->size * 2, deleter);\
    }\
    if (is_valid_deque_##type(deque))\
    {\
        if (deque->start == 0)\
            deque->start = deque->capacity - 1;\
        else\
            --deque->start;\
        *get_deque_##type(deque, 0) = data;\
        ++deque->size;\
        return 1;\
    }\
    return 0;\
}\
\
int push_back_deque_##type(struct deque_##type* deque, st_type data, void (*deleter)(st_type*, void*))\
{\
    if (deque->capacity <= deque->size + 1)\
    {\
        realloc_deque_##type(deque, deque->size * 2, deleter);\
    }\
    if (is_valid_deque_##type(deque))\
    {\
        *get_deque_##type(deque, deque->size) = data;\
        ++deque->size;\
        return 1;\
    }\
    return 0;\
}\
\
st_type* front_deque_##type(struct deque_##type* deque)\
{\
    if (deque->size == 0)\
        return NULL;\
    else\
        return get_deque_##type(deque, 0);\
}\
\
st_type* back_deque_##type(struct deque_##type* deque)\
{\
    if (deque->size == 0)\
        return NULL;\
    else\
        return get_deque_##type(deque, deque->size - 1);\
}\
\
void pop_front_deque_##type(struct deque_##type* deque)\
{\
    if (deque->size != 0)\
    {\
        --deque->size;\
        if (deque->start == deque->capacity - 1)\
            deque->start = 0;\
        else\
            ++deque->start;\
    }\
}\
\
void pop_back_deque_##type(struct deque_##type* deque)\
{\
    if (deque->size != 0)\
    {\
        --deque->size;\
    }\
}                                               \

#define PAIR(f_type, fs_type, s_type, ss_type) struct pair_##f_type##s_type \
{\
    fs_type first;                                                           \
    ss_type second;\
};\

#endif //LAB3_STRUCTS_H
