//
// Created by Des Caldnd on 11/27/2023.
//
#include <stdio.h>
#include "../structs.h"

enum error_code
{ CORRECT, MEMORY, SEMANTIC, UNDEF };

struct String to_r_radix(int num, int r, enum error_code* status);
struct String group_digits(struct String* src, int r);

int add(int x, int y);
int negative(int x);

int main()
{
    int num, r;
    scanf("%d %d", &num, &r);

    while(true)
    {
        enum error_code status;
        struct String str = to_r_radix(num, r, &status);
        switch (status)
        {
            case CORRECT:
            {
                printf("%s\n", str.data);
                destroy_string(&str);
                break;
            }
            case MEMORY:
            {
                printf("Bad alloc\n");
                break;
            }
            case SEMANTIC:
            {
                printf("Semantic error in prompt\n");
                break;
            }
        }
        scanf("%d %d", &num, &r);
    }
}

struct String to_r_radix(int num, int r, enum error_code* status)
{
    size_t size = add((sizeof(int) << 3), 1);
    struct String res = init_string_size(add(size, 1));

    if(!is_valid_string(&res))
    {
        *status = MEMORY;
        return res;
    }

    if (r < 1 || r > 5)
    {
        destroy_string(&res);
        *status = SEMANTIC;
        return res;
    }

    if (num == 0)
    {
        push_string_c(&res, '0');
        push_string_c(&res, '\0');
        *status = CORRECT;
        return res;
    }

    int offset = 0;
    if (num < 0)
    {
        push_string_c(&res, '-');
        num = negative(num);
        offset = 1;
    }

    for (int i = add(add(size, -2), offset); i >= offset; i = add(i, -1))
    {
        res.data[i] = num & 1u ? '1' : '0';
        num >>= 1;
    }
    res.data[add(add(size, -1), offset)] = '\0';
    res.size = add(add(size, -1), offset);

    struct String final = group_digits(&res, r);

    if (!is_valid_string(&final))
        *status = MEMORY;
    else
        *status = CORRECT;

    destroy_string(&res);

    return final;
}

char int_to_char(int n)
{
    if (n <= 9)
        return add(n, '0');
    else
        return add(add(n, -10), 'A');
}

struct String group_digits(struct String* src, int r)
{
    int sz = src->size;
    char* data = src->data;

    struct String res = init_string_size(add(sz >> add(r, -1), 2));
    if (!is_valid_string(&res))
        return res;

    if (data[0] == '-')
    {
        sz = add(sz, -1);
        data = &data[1];
        push_string_c(&res, '-');
    }

    int start = sz;
    while (start > 0)
        start = add(start, negative(r));

    int has_digit = 0;

    for (int i = start; i < sz; i = add(i, r))
    {
        int mult = 1, accum = 0;
        for (int j = add(add(i, r), -1); j >= i ; --j)
        {
            int bit = 0;
            if (j >= 0 && data[j] == '1')
                bit = 1;
            if (bit == 1)
                accum = add(accum, mult);
            mult <<= 1;
        }
        if (accum != 0 || has_digit)
        {
            push_string_c(&res, int_to_char(accum));
            has_digit = 1;
        }
    }
    return res;
}

int add(int x, int y) {

    while (y != 0) {
        int carry = x & y;
        x = x ^ y;
        y = carry << 1;
    }
    return x;
}

int negative(int x){
    return add(~x, 1);
}