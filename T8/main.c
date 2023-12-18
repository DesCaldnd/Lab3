//
// Created by Des Caldnd on 12/18/2023.
//
#include <stdio.h>
#include <string.h>
#include "../structs.h"
#include <ctype.h>
#include <regex.h>
#include <math.h>

/*
 * (Add|Sub|Mult|Div|Mod|Eval|Diff|Cmps)\(([1-9][0-9])+,\);
 * ((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*))([\+-](([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*))*
 * ((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*))([\+-]((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*)))*
 * ([\+-]((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*)))
 * ((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*))(([\+-]((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*)))*) works
 * (Add|Sub|Mult|Div|Mod|Diff|Cmps)\(((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*))(([\+-]((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*)))*),((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*))(([\+-]((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*)))*)\);
 * first
 *(Eval)\(((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*))(([\+-]((([1-9][0-9]*)?x(\^([1-9][0-9]*))?)|([1-9][0-9]*)))*),([1-9][0-9]*)(.([0-9]*[1-9]))?\);
 * eval
 */

enum oper_type
{ ADD, SUB, MULT, DIV, MOD, EVAL, DIFF, CMPS, UNDEF };

PAIR(s, size_t, i, int)

LIST(m, struct pair_si)

int is_needed_sym(int c);
int no_skip_sym(int c);
enum oper_type get_oper_type(struct String str);
size_t get_arg_count(struct String str);
int parse_and_execute_line(struct String str, struct list_m* list);
void skip_line_comment(FILE* in);
void skip_multiline_comment(FILE* in);
void process_file(FILE* in);
int check_line(struct String str, size_t arg_count);
int check_eval_line(struct String str, size_t arg_count);
size_t args_start_position(struct String str);
struct list_m parse_polynomial(struct String str, size_t start_pos, size_t* end_pos);
double eval(struct list_m f_polynomial, double dot);
struct list_m add(struct list_m f_polynomial, struct list_m s_polynomial);
struct list_m sub(struct list_m f_polynomial, struct list_m s_polynomial);
struct list_m mult(struct list_m f_polynomial, struct list_m s_polynomial);
struct list_m _div(struct list_m f_polynomial, struct list_m s_polynomial);
struct list_m mod(struct list_m f_polynomial, struct list_m s_polynomial);
struct list_m diff(struct list_m polynomial);
struct list_m cmps(struct list_m f_polynomial, struct list_m s_polynomial);
void print_polynomial(struct list_m polynomial);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Incorrect argc\n");
        return 0;
    }
    FILE* in = fopen(argv[1], "r");

    if (in == NULL)
    {
        printf("Input file cannot be opened\n");
    }

    process_file(in);

    fclose(in);
    return 0;
}

int is_needed_sym(int c)
{
    return !isspace(c) && !no_skip_sym(c);
}

int no_skip_sym(int c)
{
    return c == '%' || c == '[' || c == ']';
}

enum oper_type get_oper_type(const struct String str)
{
    if (strncmp(str.data, "Add", 3) == 0)
        return ADD;
    else if (strncmp(str.data, "Sub", 3) == 0)
        return SUB;
    else if (strncmp(str.data, "Mult", 4) == 0)
        return MULT;
    else if (strncmp(str.data, "Div", 3) == 0)
        return DIV;
    else if (strncmp(str.data, "Mod", 3) == 0)
        return MOD;
    else if (strncmp(str.data, "Eval", 4) == 0)
        return EVAL;
    else if (strncmp(str.data, "Diff", 3) == 0)
        return DIFF;
    else if (strncmp(str.data, "Cmps", 4) == 0)
        return CMPS;
    else
        return UNDEF;
}

int parse_and_execute_line(const struct String str, struct list_m* list)
{
    enum oper_type operation = get_oper_type(str);

    if (operation == UNDEF)
    {
        printf("Unknown operation in following line:\n\t%s\n", str.data);
        return 2;
    }

    int (*checker)(const struct String, size_t) = check_line;

    if (operation == EVAL)
    {
        checker = check_eval_line;
    }

    size_t arg_count = get_arg_count(str);

    if (!((arg_count <= 1 && operation == DIFF) || (arg_count >= 1 && arg_count <= 2 && operation != DIFF)))
    {
        printf("Incorrect argument count in following line:\n\t%s\n", str.data);
        return 4;
    }

    if (!checker(str, arg_count))
    {
        printf("Syntax error in following line:\n\t%s\n", str.data);
        return 3;
    }

    struct list_m first_arg;
    size_t start_pos = args_start_position(str);
    if (arg_count == 2 || (arg_count == 1 && operation == DIFF))
    {
        first_arg = parse_polynomial(str, start_pos, &start_pos);
    } else
    {
        first_arg = copy_list_m(*list);
    }
    if (!is_list_valid_m(first_arg))
        return 0;


    if (operation == EVAL)
    {
        char* end;
        double second_arg = strtod(str.data + start_pos, &end);
        double result = eval(first_arg, second_arg);
        printf("%.2lf\n", result);
    } else if (operation == DIFF)
    {
        struct list_m result = diff(first_arg);
        if (!is_list_valid_m(result))
        {
            destroy_list_m(&first_arg);
            return 0;
        } else
        {
            print_polynomial(result);
            destroy_list_m(list);
            *list = result;
        }
    } else
    {
        struct list_m result;
        struct list_m second_arg = parse_polynomial(str, start_pos, &start_pos);
        if (!is_list_valid_m(second_arg))
        {
            destroy_list_m(&first_arg);
            return 0;
        }

        switch (operation)
        {
            case ADD:
                result = add(first_arg, second_arg);
                break;
            case SUB:
                result = sub(first_arg, second_arg);
                break;
            case MULT:
                result = mult(first_arg, second_arg);
                break;
            case DIV:
                if (second_arg.begin->next == NULL)
                {
                    printf("Division by zero exception\n");
                    destroy_list_m(&second_arg);
                    destroy_list_m(&first_arg);
                    destroy_list_m(&result);
                    return 5;
                }
                result = _div(first_arg, second_arg);
                break;
            case MOD:
                if (second_arg.begin->next == NULL)
                {
                    printf("Division by zero exception\n");
                    destroy_list_m(&second_arg);
                    destroy_list_m(&first_arg);
                    destroy_list_m(&result);
                    return 5;
                }
                result = mod(first_arg, second_arg);
                break;
            case CMPS:
                result = cmps(first_arg, second_arg);
                break;
        }

        destroy_list_m(&second_arg);
        if (!is_list_valid_m(result))
        {
            destroy_list_m(&first_arg);
            return 0;
        } else
        {
            print_polynomial(result);
            destroy_list_m(list);
            *list = result;
        }

    }

    destroy_list_m(&first_arg);

    return 1;
}

void process_file(FILE* in)
{
    struct list_m summator = init_list_m();
    if (!is_list_valid_m(summator))
    {
        printf("Error in memory allocation\n");
    }

    while (!feof(in))
    {
        char buf;
        struct String expr = init_string_from_stream_buf(in, &buf, is_needed_sym, no_skip_sym);

        if (!is_valid_string(&expr))
        {
            printf("Error parsing line caused by memory limit\n");
            destroy_list_m(&summator);
            return;
        }

        if (!is_empty_string(&expr))
        {
            if (!parse_and_execute_line(expr, &summator))
            {
                printf("Error executing line caused by memory limit\n");
                destroy_list_m(&summator);
                return;
            }
        }

        if (!is_list_valid_m(summator))
        {
            printf("Error in memory allocation\n");
            return;
        }

        switch (buf)
        {
            case '%':
                skip_line_comment(in);
                break;
            case '[':
                skip_multiline_comment(in);
                break;
        }

        destroy_string(&expr);
    }
    destroy_list_m(&summator);
}

void skip_line_comment(FILE* in)
{
    while (getc(in) != '\n' && !feof(in)){}
}

void skip_multiline_comment(FILE* in)
{
    while (getc(in) != ']' && !feof(in)){}
}

size_t get_arg_count(const struct String str)
{
    size_t res = 0, ind = 0;

    while (ind < str.size && str.data[ind] != '(')
    {
        ++ind;
    }
    ++ind;
    int can_increase = 1;

    while (ind < str.size && str.data[ind] != ')')
    {
        if (str.data[ind] == ',')
            can_increase = 1;
        else if (can_increase)
        {
            ++res;
            can_increase = 0;
        }
        ++ind;
    }
    return res;
}

int check_line(const struct String str, size_t arg_count)
{
    regex_t line_regex;

    int result;
    if (arg_count == 2)
        result = regcomp(&line_regex, "(Add|Sub|Mult|Div|Mod|Diff|Cmps)\\(((([1-9][0-9]*)?x(\\^([1-9][0-9]*))?)|([1-9][0-9]*))"
                                  "(([\\+-]((([1-9][0-9]*)?x(\\^([1-9][0-9]*))?)|([1-9][0-9]*)))*),"
                                  "((([1-9][0-9]*)?x(\\^([1-9][0-9]*))?)|([1-9][0-9]*))"
                                  "(([\\+-]((([1-9][0-9]*)?x(\\^([1-9][0-9]*))?)|([1-9][0-9]*)))*)\\);", REG_EXTENDED);
    else if (arg_count == 1)
        result = regcomp(&line_regex, "(Add|Sub|Mult|Div|Mod|Diff|Cmps)\\("
                                      "((([1-9][0-9]*)?x(\\^([1-9][0-9]*))?)|([1-9][0-9]*))"
                                      "(([\\+-]((([1-9][0-9]*)?x(\\^([1-9][0-9]*))?)|([1-9][0-9]*)))*)\\);", REG_EXTENDED);
    else
        result = regcomp(&line_regex, "(Add|Sub|Mult|Div|Mod|Diff|Cmps)\\(\);", REG_EXTENDED);

    if (!(result == 0))
        return 0;
    result = regexec(&line_regex, str.data, 0, NULL, 0);
    regfree(&line_regex);

    return result == 0;
}

int check_eval_line(const struct String str, size_t arg_count)
{
    regex_t line_regex;

    int result;
    if (arg_count == 2)
    result = regcomp(&line_regex, "(Eval)\\(((([1-9][0-9]*)?x(\\^([1-9][0-9]*))?)|([1-9][0-9]*))(([\\+-]((([1-9][0-9]*)?x"
                                  "(\\^([1-9][0-9]*))?)|([1-9][0-9]*)))*),[0-9]+(.([0-9]*[1-9]))?\\);", REG_EXTENDED);
    else
        result = regcomp(&line_regex, "(Eval)\\([0-9]+(.([0-9]*[1-9]))?\\);", REG_EXTENDED);

    if (!(result == 0))
        return 0;
    result = regexec(&line_regex, str.data, 0, NULL, 0);
    regfree(&line_regex);

    return result == 0;
}

size_t args_start_position(const struct String str)
{
    size_t result = 0;
    while (str.data[result] != '(')
        ++result;
    ++result;
    return result;
}

struct pair_si parse_member(const struct String str, size_t start_pos, size_t* end_pos)
{
    char* end;
    int coeff = strtol(str.data + start_pos, &end, 10);
    size_t power = 0;

    if (str.data + start_pos == end)
    {
        coeff = str.data[start_pos] == '-' ? -1 : 1;
        if (str.data[start_pos] == '-' || str.data[start_pos] == '+')
            ++start_pos;
    } else
    {
        start_pos += end - str.data - start_pos;
    }
    if (str.data[start_pos] == 'x')
    {
        power = 1;
        ++start_pos;
        if (str.data[start_pos] == '^')
        {
            ++start_pos;
            power = strtoull(str.data + start_pos, &end, 10);
            start_pos += end - str.data - start_pos;
        }
    }
    *end_pos = start_pos;
    struct pair_si pair = {power, coeff};
    return pair;
}

int insert_member(struct list_m* polynomial, struct pair_si member)
{
    struct node_m* prev = polynomial->begin, *cur = prev->next;

    while(cur != NULL && member.first < cur->data.first)
    {
        cur = cur->next;
        prev = prev->next;
    }

    struct node_m* new_node = cur;
    if (cur == NULL || cur->data.first != member.first)
    {
        new_node = insert_list_m(prev, member);
        if (new_node == NULL)
            return 0;
    } else
    {
        new_node->data.second += member.second;
    }
    if (new_node->data.second == 0)
        remove_list_m(prev);

    return 1;
}

struct list_m parse_polynomial(const struct String str, size_t start_pos, size_t* end_pos)
{
    struct list_m polynomial = init_list_m();
    if (!is_list_valid_m(polynomial))
        return polynomial;

    while (str.data[start_pos] != ',' && str.data[start_pos] != ')')
    {
        struct pair_si member = parse_member(str, start_pos, &start_pos);
        if (member.second == 0)
            continue;
        if (!insert_member(&polynomial, member))
        {
            destroy_list_m(&polynomial);
            return polynomial;
        }
    }


    *end_pos = start_pos + 1;
    return polynomial;
}

double eval(struct list_m f_polynomial, double dot)
{
    struct node_m* it = f_polynomial.begin->next;
    double accum = 0;
    while (it != NULL)
    {
        accum += it->data.second * pow(dot, it->data.first);
        it = it->next;
    }
    return accum;
}

struct node_m *find_member_by_power(struct list_m polynomial, size_t power)
{
    struct node_m *it = polynomial.begin->next;

    while(it != NULL && it->data.first > power)
        it = it->next;
    return it != NULL && it->data.first == power ? it : NULL;
}

struct list_m add(struct list_m f_polynomial, struct list_m s_polynomial)
{
    struct list_m result = copy_list_m(f_polynomial);
    if (!is_list_valid_m(result))
        return result;

    struct node_m* it = s_polynomial.begin->next;
    while(it != NULL)
    {
        struct node_m* tmp = find_member_by_power(result, it->data.first);
        if (tmp == NULL)
        {
            struct pair_si member;
            member.first = it->data.first;
            member.second = it->data.second;
            int status = insert_member(&result, member);
            if (!status)
            {
                destroy_list_m(&result);
                return result;
            }
        } else
        {
            tmp->data.second += it->data.second;
            if (tmp->data.second == 0)
            {
                struct node_m* tmp_node = result.begin;
                while (tmp_node->next != tmp)
                    tmp_node = tmp_node->next;
                remove_list_m(tmp_node);
            }
        }
        it = it->next;
    }
    return result;
}

struct list_m sub(struct list_m f_polynomial, struct list_m s_polynomial)
{
    struct list_m result = copy_list_m(f_polynomial);
    if (!is_list_valid_m(result))
        return result;

    struct node_m* it = s_polynomial.begin->next;
    while(it != NULL)
    {
        struct node_m* tmp = find_member_by_power(result, it->data.first);
        if (tmp == NULL)
        {
            struct pair_si member;
            member.first = it->data.first;
            member.second = -it->data.second;
            int status = insert_member(&result, member);
            if (!status)
            {
                destroy_list_m(&result);
                return result;
            }
        } else
        {
            tmp->data.second -= it->data.second;
            if (tmp->data.second == 0)
            {
                struct node_m* tmp_node = result.begin;
                while (tmp_node->next != tmp)
                    tmp_node = tmp_node->next;
                remove_list_m(tmp_node);
            }
        }
        it = it->next;
    }
    return result;
}

struct list_m mult_by_member(struct list_m polynomial, struct pair_si member)
{
    struct list_m result = copy_list_m(polynomial);
    if (!is_list_valid_m(result))
        return result;

    struct node_m* it = result.begin->next;

    while(it != NULL)
    {
        it->data.first += member.first;
        it->data.second *= member.second;
        it = it->next;
    }
    return result;
}

struct list_m mult(struct list_m f_polynomial, struct list_m s_polynomial)
{
    struct list_m result = init_list_m();
    if (!is_list_valid_m(result))
        return result;

    struct node_m* it = s_polynomial.begin->next;

    while(it != NULL)
    {
        struct list_m mult_res = mult_by_member(f_polynomial, it->data);
        if (!is_list_valid_m(mult_res))
        {
            destroy_list_m(&result);
            return result;
        }
        struct list_m add_res = add(result, mult_res);
        destroy_list_m(&mult_res);
        if (!is_list_valid_m(add_res))
        {
            destroy_list_m(&result);
        }
        destroy_list_m(&result);
        result = add_res;
        it = it->next;
    }
    return result;
}

struct list_m _div(struct list_m f_polynomial, struct list_m s_polynomial)
{
    if (f_polynomial.begin->next->data.first < s_polynomial.begin->next->data.first)
        return init_list_m();

    struct list_m result = init_list_m();
    if (!is_list_valid_m(result))
        return result;

    struct list_m numerator = copy_list_m(f_polynomial);

    if (!is_list_valid_m(numerator))
    {
        destroy_list_m(&result);
        return result;
    }

    size_t max_power = f_polynomial.begin->next->data.first;
    int need_it = 1;

    while(max_power > 0 || need_it)
    {
        struct node_m* it = numerator.begin->next;
        while(it != NULL && it->data.first > max_power)
            it = it->next;
        if (it == NULL)
            break;
        max_power = it->data.first;
        if (max_power < s_polynomial.begin->next->data.first)
            break;

        struct node_m* prev = numerator.begin;
        while (prev->next != it)
            prev = prev->next;

        struct node_m* divider = s_polynomial.begin->next;

        size_t power = it->data.first - divider->data.first;
        int coeff = it->data.second / divider->data.second;

        while (it != NULL && divider != NULL)
        {
            while (it != NULL && it->data.first > divider->data.first + power)
            {
                prev = prev->next;
                it = it->next;
            }
            if (it == NULL)
                break;
            if (it->data.first == divider->data.first + power)
            {
                it->data.second -= divider->data.second * coeff;
                if (it->data.second == 0)
                {
                    it = it->next;
                    remove_list_m(prev);
                } else
                {
                    it = it->next;
                    prev = prev->next;
                }
            } else
            {
                struct pair_si member;
                member.first = divider->data.first + power;
                member.second = -divider->data.second * coeff;
                struct node_m* status = insert_list_m(prev, member);

                if (status == NULL)
                {
                    destroy_list_m(&result);
                    destroy_list_m(&numerator);
                    return result;
                }

                prev = status;
            }
            divider = divider->next;
        }

        struct pair_si member = {power, coeff};
        int status = insert_member(&result, member);
        if (!status)
        {
            destroy_list_m(&result);
            destroy_list_m(&numerator);
            return result;
        }

        if (max_power > 0)
            --max_power;
        else
            need_it = 0;
    }


    destroy_list_m(&numerator);
    return result;
}

struct list_m mod(struct list_m f_polynomial, struct list_m s_polynomial)
{
    struct list_m div_res = _div(f_polynomial, s_polynomial);
    if (!is_list_valid_m(div_res))
        return div_res;
    struct list_m mult_res = mult(div_res, s_polynomial);
    destroy_list_m(&div_res);
    if (!is_list_valid_m(mult_res))
        return mult_res;
    struct list_m sub_res = sub(f_polynomial, mult_res);
    destroy_list_m(&mult_res);
    return sub_res;
}

struct list_m diff(struct list_m polynomial)
{
    struct list_m result = copy_list_m(polynomial);
    if (!is_list_valid_m(polynomial))
        return result;
    struct node_m* prev = result.begin, *cur = prev->next;

    while(cur != NULL)
    {
        if (cur->data.first != 0)
        {
            cur->data.second *= cur->data.first;
            --cur->data.first;
        } else
        {
            remove_list_m(prev);
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    return result;
}

struct list_m cmps(struct list_m f_polynomial, struct list_m s_polynomial)
{
    struct list_m result = init_list_m();
    if (!is_list_valid_m(result))
        return result;

    struct node_m* it = f_polynomial.begin->next;

    while (it != NULL)
    {
        struct list_m l_multiplier;
        if (it->data.first == 0)
            l_multiplier = init_list_m();
        else
            l_multiplier = copy_list_m(s_polynomial);
        if (!is_list_valid_m(l_multiplier))
        {
            destroy_list_m(&result);
            return result;
        }
        if (it->data.first == 0)
        {
            struct pair_si member = {0, 1};
            int state = insert_member(&l_multiplier, member);
            if (!state)
            {
                destroy_list_m(&l_multiplier);
                destroy_list_m(&result);
                return result;
            }
        }
        for (int i = 1; i < it->data.first; ++i)
        {
            struct list_m mult_res = mult(l_multiplier, s_polynomial);
            destroy_list_m(&l_multiplier);
            if (!is_list_valid_m(mult_res))
            {
                destroy_list_m(&result);
                return result;
            }
            l_multiplier = mult_res;
        }
        struct node_m* r_it = l_multiplier.begin->next;

        while (r_it != NULL)
        {
            r_it->data.second *= it->data.second;
            r_it = r_it->next;
        }

        struct list_m add_res = add(result, l_multiplier);
        destroy_list_m(&l_multiplier);
        destroy_list_m(&result);
        if (!is_list_valid_m(add_res))
            return result;

        result = add_res;

        it = it->next;
    }
    return result;
}

void print_member(struct pair_si pair, int need_sign)
{
    int coeff = pair.second;
    size_t power = pair.first;

    if (coeff == 0)
        return;

    if (need_sign || coeff < 0)
        printf("%c", coeff < 0 ? '-' : '+');

    coeff = abs(coeff);
    if (coeff != 1 || power == 0)
        printf("%d", coeff);

    if (power == 0)
        return;

    if (power == 1)
        printf("x");
    else
        printf("x^%zu", power);
}

void print_polynomial(struct list_m polynomial)
{
    struct node_m* it = polynomial.begin->next;
    if (it == NULL)
    {
        printf("0\n");
        return;
    }
    print_member(it->data, 0);
    it = it->next;
    while (it != NULL)
    {
        print_member(it->data, 1);
        it = it->next;
    }
    printf("\n");
}