//
// Created by Des Caldnd on 12/16/2023.
//
#include <stdio.h>
#include <ctype.h>
#include "../structs.h"

struct node;

LIST(nodep, struct node*)

struct node
{
    struct node* parent;
    char data;
    struct list_nodep children;
};

PAIR(file, FILE*, int, int)

int parse_print_expressions(FILE* in, FILE* out);
int not_delemeter(int sym);
void delete_tree(struct node** n, void*);
struct node* parse_tree(const struct String str);
void print_node(struct node** n, void* opt_data);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Incorrect argc\n");
        return 0;
    }

    FILE* in = fopen(argv[1], "r");

    if (in == NULL)
    {
        printf("Error while opening input file\n");
        return 0;
    }

    FILE* out = fopen(argv[2], "w");

    if (out == NULL)
    {
        printf("Error while opening output file\n");
        fclose(in);
        return 0;
    }

    if (parse_print_expressions(in, out))
    {
        printf("SUCCESS\n");
    } else
    {
        printf("FAIL\n");
    }

    fclose(in);
    fclose(out);
    return 0;
}

int parse_print_expressions(FILE* in, FILE* out)
{
    struct String expr;

    while (!feof(in))
    {
        expr = init_string_from_stream(in, not_delemeter);
        if (!is_valid_string(&expr))
            return 0;

        struct node* tree = parse_tree(expr);
        if (tree != NULL)
        {
            struct pair_fileint pair;
            pair.first = out;
            pair.second = 0;
            print_node(&tree, &pair);
            delete_tree(&tree, NULL);
        }

        destroy_string(&expr);
    }

    return 1;
}

int not_delemeter(int sym)
{
    return sym != EOF && sym != '\n';
}

void delete_tree(struct node** n, void* a)
{
    if (*n == NULL)
        return;
    for_all_list_nodep((*n)->children, delete_tree, NULL);
    destroy_list_nodep(&(*n)->children);
    free(*n);
}

struct node* create_node(int data, struct node* parent)
{
    struct node* return_node = malloc(sizeof(struct node));
    if (return_node == NULL)
        return NULL;
    return_node->children = init_list_nodep();
    if (!is_list_valid_nodep(return_node->children))
    {
        free(return_node);
        return NULL;
    }
    return_node->data = data;
    return_node->parent = parent;
    return return_node;
}

struct node* parse_tree(const struct String str)
{
    struct node* gen_node = create_node(0, NULL);
    struct node* it_node = gen_node;

    for (int i = 0; i < str.size; ++i)
    {
        char sym = str.data[i];

        if (sym == '(')
            it_node = it_node->children.begin->next->data;
        if (sym == ')')
            it_node = it_node->parent;
        if (isalpha(sym))
        {
            struct node* next = create_node(sym, it_node);
            if (next == NULL)
            {
                delete_tree(&gen_node, NULL);
                return NULL;
            }
            struct node_nodep* status = insert_list_nodep(it_node->children.begin, next);
            if (status == NULL)
            {
                delete_tree(&gen_node, NULL);
                free(next);
                return NULL;
            }
        }
    }

    it_node = gen_node->children.begin->next->data;

    destroy_list_nodep(&gen_node->children);
    free(gen_node);
    return it_node;
}

void print_node(struct node** n, void* opt_data)
{
    struct pair_fileint pair = *((struct pair_fileint*) opt_data);
    for (int i = 0; i < pair.second; ++i)
    {
        putc('\t', pair.first);
    }
    fprintf(pair.first, "%c\n", (*n)->data);
    ++pair.second;
    for_all_list_nodep((*n)->children, print_node, &pair);
}