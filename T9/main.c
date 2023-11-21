//
// Created by Des Caldnd on 11/11/2023.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WORD_LENGTH 40
#define STR2(x) #x
#define STR(X) STR2(X)

struct item
{
    char word[WORD_LENGTH + 1];
    int length;
    int count;
};

enum error_type
{ CORRECT, ERROR };

enum command_type
{ GET, FIRST, MAX, MIN, SAVE, DEPTH, QUIT, UNDEF };

struct node
{
    struct item data;
    struct node *l, *r;
};

struct array
{
    struct item* data;
    int size, capacity;
};

enum command_type get_command(char* str);

struct node* parse_text(char* filepath, int sep_count, char* separators);
struct node* parse_tree(char* filepath);

struct item* find(struct node* root, char* word);
int insert(struct node* root, struct item data);
void destroy(struct node* root);
struct array get_first(struct node* root, int n);
struct item* get_max(struct node* root);
struct item* get_min(struct node* root);
enum error_type save_tree(struct node* root, char* path);
int get_depth(struct node* root);
int max_i(int left, int right);

void save_node(struct node* root, FILE* out);

void get_first_rec(struct node* root, struct array* res);
void array_insert(struct array* res, struct item data);

int get_token(FILE* in, char* token, int sep_count, char* separators, char* c);
int is_in(char sym, int sep_count, char* separators);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Incorrect arguments. You must pass .txt or .bst filepath\n");
        return 0;
    }
    struct node* tree_head = NULL;
    if (strncmp(argv[1] + strlen(argv[1]) - 4, ".txt", 4) == 0)
    {
        if (argc < 3)
        {
            printf("You must enter list of separators\n");
            return 0;
        }
        char* separators = malloc(sizeof(char) * (argc - 2));
        if (separators == NULL)
        {
            printf("Bad alloc");
            return 0;
        }
        for (int i = 2; i < argc; ++i)
        {
            separators[i - 2] = argv[i][0];
        }
        tree_head = parse_text(argv[1], argc - 2, separators);
        free(separators);
    } else if (strncmp(argv[1] + strlen(argv[1]) - 4, ".bst", 4) == 0)
    {
        tree_head = parse_tree(argv[1]);
    } else
    {
        printf("Incorrect filepath. Extensions must be: .txt or .bst\n");
        return 0;
    }

    if (tree_head == NULL)
    {
        printf("Some error occurred. Maybe filepath is incorrect or file is empty\n");
        return 0;
    }

    char prompt[20];
    scanf("%19s", prompt);
    enum command_type type;
    while ((type = get_command(prompt)) != QUIT)
    {
        switch (type)
        {
            case GET:
            {
                char word[WORD_LENGTH + 1];
                scanf("%" STR(WORD_LENGTH) "s", word);
                struct item *data = find(tree_head, word);
                if (data != NULL)
                    printf("Word %s has %d ascensions\n", word, data->count);
                else
                    printf("There is no such word\n");
                break;
            }
            case FIRST:
            {
                int n;
                int count = scanf("%d", &n);
                if (count == 1 && n >= 1)
                {
                    struct array data_ar = get_first(tree_head, n);
                    if (data_ar.data != NULL)
                    {
                        printf("%s", data_ar.data[0].word);
                        for (int i = 1; i < data_ar.size; ++i)
                        {
                            printf(" %s", data_ar.data[i].word);
                        }
                        printf("\n");
                        free(data_ar.data);
                    } else
                    {
                        printf("Some error occurred. Maybe bad alloc\n");
                    }
                }
                break;
            }
            case MAX:
            {
                struct item* data = get_max(tree_head);
                printf("%s\n", data->word);
                break;
            }
            case MIN:
            {
                struct item* data = get_min(tree_head);
                printf("%s\n", data->word);
                break;
            }
            case SAVE:
            {
                char dest_path[256];
                scanf("%255s", dest_path);
                enum error_type result = save_tree(tree_head, dest_path);
                if (result == CORRECT)
                    printf("SUCCESS\n");
                else
                    printf("FAIL\n");
                break;
            }
            case DEPTH:
            {
                int depth = get_depth(tree_head);
                printf("Tree depth is: %d\n", depth);
                break;
            }
            default:
            {
                printf("No such command\n");
                break;
            }
        }
        scanf("%19s", prompt);
    }
    destroy(tree_head);
    return 0;
}

enum command_type get_command(char* str)
{
    if (strcmp(str, "get") == 0)
        return GET;
    else if (strcmp(str, "first") == 0)
        return FIRST;
    else if (strcmp(str, "max") == 0)
        return MAX;
    else if (strcmp(str, "min") == 0)
        return MIN;
    else if (strcmp(str, "save") == 0)
        return SAVE;
    else if (strcmp(str, "depth") == 0)
        return DEPTH;
    else if (strcmp(str, "quit") == 0)
        return QUIT;
    else
        return UNDEF;
}

struct node* parse_text(char* filepath, int sep_count, char* separators)
{
    FILE* in = fopen(filepath, "r");
    if(in == NULL)
        return NULL;
    struct node* root = malloc(sizeof(struct node));
    if (root == NULL)
    {
        fclose(in);
        return NULL;
    }

    char token[WORD_LENGTH + 1];
    int length;

    char c = fgetc(in);
    length = get_token(in, token, sep_count, separators, &c);
    strncpy(root->data.word, token, length + 1);
    root->data.length = length;
    root->data.count = 1;
    root->data.count = 1;
    root->l = NULL;
    root->r = NULL;

    length = get_token(in, token, sep_count, separators, &c);
    while (length > 0)
    {
        struct item data;
        data.length = length;
        data.count = 1;
        strncpy(data.word, token, length + 1);
        if (!insert(root, data))
        {
            fclose(in);
            destroy(root);
            return NULL;
        }
        length = get_token(in, token, sep_count, separators, &c);
    }

    fclose(in);
    return root;
}

int get_token(FILE* in, char* token, int sep_count, char* separators, char* c)
{
    int length = 0;

    while (is_in(*c, sep_count, separators) && *c != EOF)
        *c = fgetc(in);

    while(length < WORD_LENGTH && !is_in(*c, sep_count, separators) && *c != EOF)
    {
        token[length] = *c;
        ++length;
        *c = fgetc(in);
    }

    token[length] = '\0';

    return length;
}

int is_in(char sym, int sep_count, char* separators)
{
    for (int i = 0; i < sep_count; ++i)
    {
        if (separators[i] == sym)
            return 1;
    }
    return 0;
}

struct node* parse_tree(char* filepath)
{
    FILE* in = fopen(filepath, "rb");
    if(in == NULL)
        return NULL;
    struct node* root = malloc(sizeof(struct node));
    if (root == NULL)
    {
        fclose(in);
        return NULL;
    }

    struct item data;
    fread(&data.length, sizeof(int), 1, in);
    fread(&data.count, sizeof(int), 1, in);
    fread(data.word, sizeof(char ), data.length, in);
    root->data = data;
    root->l = NULL;
    root->r = NULL;
    while (fread(&data.length, sizeof(int), 1, in) > 0)
    {
        fread(&data.count, sizeof(int), 1, in);
        fread(data.word, sizeof(char), data.length, in);
        data.word[data.length] = '\0';
        int status = insert(root, data);
        if (!status)
        {
            destroy(root);
            fclose(in);
            return NULL;
        }
    }

    fclose(in);
    return root;
}

struct item* find(struct node* root, char* word)
{
    int comp = strcmp(word, root->data.word);
    if (comp == 0)
    {
        return &root->data;
    }
    if (comp == -1 && root->l != NULL)
        return find(root->l, word);
    else if (comp == 1 && root->r != NULL)
        return find(root->r, word);
    return NULL;
}

int insert(struct node* root, struct item data)
{
    int comp = strcmp( data.word, root->data.word);
    if (comp == 0)
    {
        ++root->data.count;
        return 1;
    }
    if (comp < 0)
    {
        if (root->l != NULL)
            return insert(root->l, data);

        struct node* tmp = malloc(sizeof(struct node));
        if (tmp == NULL)
            return 0;
        tmp->l = NULL;
        tmp->r = NULL;
        tmp->data = data;
        root->l = tmp;
    } else if (comp > 0)
    {
        if (root->r != NULL)
            return insert(root->r, data);

        struct node* tmp = malloc(sizeof(struct node));
        if (tmp == NULL)
            return 0;
        tmp->l = NULL;
        tmp->r = NULL;
        tmp->data = data;
        root->r = tmp;
    }
    return 1;
}

void destroy(struct node* root)
{
    if (root != NULL)
    {
        free(root);
        destroy(root->l);
        destroy(root->r);
    }
}

struct array get_first(struct node* root, int n)
{
    struct array res;
    res.data = malloc(sizeof(struct item) * n);
    if (res.data == NULL)
    {
        return res;
    }
    res.size = 0;
    res.capacity = n;

    get_first_rec(root, &res);

    return res;
}

void get_first_rec(struct node* root, struct array* res)
{
    array_insert(res, root->data);
    if (root->l != NULL)
        get_first_rec(root->l, res);
    if (root->r != NULL)
        get_first_rec(root->r, res);
}

void array_insert(struct array* res, struct item data)
{
    int counter = 0;
    while (counter < res->size && data.length <= res->data[counter].length)
        ++counter;

    if (counter == res->capacity)
        return;
    if (res->size < res->capacity)
        ++res->size;

    struct item tmp;
    for (int i = counter; i < res->size; ++i)
    {
        tmp = res->data[i];
        res->data[i] = data;
        data = tmp;
    }
}

struct item* get_max(struct node* root)
{
    struct item* res = &root->data;
    if (root->l != NULL)
    {
        struct item* tmp = get_max(root->l);
        if (tmp->length > res->length)
            res = tmp;
    }
    if (root->r != NULL)
    {
        struct item* tmp = get_max(root->r);
        if (tmp->length > res->length)
            res = tmp;
    }

    return res;
}

struct item* get_min(struct node* root)
{
    struct item* res = &root->data;
    if (root->l != NULL)
    {
        struct item* tmp = get_min(root->l);
        if (tmp->length < res->length)
            res = tmp;
    }
    if (root->r != NULL)
    {
        struct item* tmp = get_min(root->r);
        if (tmp->length < res->length)
            res = tmp;
    }

    return res;
}

enum error_type save_tree(struct node* root, char* path)
{
    FILE *out = fopen(path, "wb");
    if (out == NULL)
        return ERROR;

    save_node(root, out);

    fclose(out);
    return CORRECT;
}

void save_node(struct node* root, FILE* out)
{
    if (root == NULL)
        return;

    fwrite(&root->data.length, sizeof(int), 1, out);
    fwrite(&root->data.count, sizeof(int), 1, out);
    fwrite(root->data.word, sizeof(char), root->data.length, out);

    save_node(root->l, out);
    save_node(root->r, out);
}

int get_depth(struct node* root)
{
    int result = 0;
    if (root != 0)
    {
        ++result;
        int left = get_depth(root->l);
        int right = get_depth(root->r);
        result += max_i(left, right);
    }
    return result;
}

int max_i(int left, int right)
{
    return left > right ? left : right;
}
