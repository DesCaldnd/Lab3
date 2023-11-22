//
// Created by Des Caldnd on 11/21/2023.
//

#ifndef LAB3_STRUCTS_H
#define LAB3_STRUCTS_H
#include <stdlib.h>

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

#endif //LAB3_STRUCTS_H
