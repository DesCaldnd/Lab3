#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

#define EPS 1E-6

struct vectors
{
    double **arr;
    int size;
    int capacity;
};

typedef double(*vec_norm)(double*, int, void*);

struct vectors max_vector_norms(int n, int vector_count, int norm_count, ...);

bool d_equal(double lhs, double rhs);

void vector_init(struct vectors* vector);
bool vector_check(struct vectors vector);
void vector_free(struct vectors* vector);
void vector_realloc(struct vectors* vector);
void vector_push_back(struct vectors* vector, double * val);

double norm_1(double* arr, int n, void* data);
double norm_2(double* arr, int n, void* data);
double norm_3(double* arr, int n, void* data);


int main()
{
    struct vectors res;

    vector_free(&res);
    return 0;
}


void vector_init(struct vectors* vector)
{
    vector->arr = malloc(10 * sizeof(double *));
    vector->size = 0;
    vector->capacity = 10;
}

bool vector_check(struct vectors vector)
{
    return vector.arr != NULL;
}

void vector_free(struct vectors* vector)
{
    free(vector->arr);
}

void vector_realloc(struct vectors* vector)
{
    vector->arr = realloc(vector->arr, vector->capacity * 2);
    vector->capacity *= 2;
}

void vector_push_back(struct vectors* vector, double * val)
{
    if (vector->size >= vector->capacity)
    {
        vector_realloc(vector);
    }
    if (!vector_check(*vector))
        return;

    vector->arr[vector->size] = val;
    ++vector->size;
}

struct vectors max_vector_norms(int n, int vector_count, int norm_count, ...)
{
    struct vectors res;
    vector_init(&res);
    if (!vector_check(res) || vector_count < 1 || norm_count < 1)
        return res;

    double **all_vecs = malloc(n * sizeof(double *));
    if (all_vecs == NULL)
    {
        vector_free(&res);
        return res;
    }
    int *indicies = malloc(n * sizeof(int));
    int ind_written;
    if (indicies == NULL)
    {
        vector_free(&res);
        free(all_vecs);
        return res;
    }

    va_list args;
    va_start(args, norm_count);

    for (int i = 0; i < vector_count; ++i)
    {
        all_vecs[i] = va_arg(args, double *);
    }

    for (int i = 0; i < norm_count; ++i)
    {
        vec_norm function = va_arg(args, vec_norm);
        void * data = va_arg(args, void*);
        double max_length = function(all_vecs[0], n, data);
        indicies[0] = 0;
        ind_written = 1;

        for (int j = 1; j < vector_count; ++j)
        {
            double length = function(all_vecs[j], n, data);
            if (d_equal(length, max_length))
            {
                 indicies[ind_written] = i;
                 ++ind_written;
            } else if (length > max_length)
            {
                max_length = length;
                indicies[0] = i;
                ind_written = 1;
            }
        }

        for (int j = 0; j < ind_written; ++j)
        {
            vector_push_back(&res, all_vecs[indicies[j]]);
            if (!vector_check(res))
            {
                free(all_vecs);
                free(indicies);
                return res;
            }
        }
    }




    free(all_vecs);
    free(indicies);
    return res;
}

bool d_equal(double lhs, double rhs)
{
    if (fabs(lhs - rhs) < EPS)
        return true;
    else
        return false;
}

double norm_1(double* arr, int n, void* data)
{

}

double norm_2(double* arr, int n, void* data)
{

}

double norm_3(double* arr, int n, void* data)
{

}