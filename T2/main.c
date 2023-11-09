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
void matix_vec_prod(double **matrix, double *vec, double* res, int n);

void vector_init(struct vectors* vector);
bool vector_check(struct vectors vector);
void vector_free(struct vectors* vector);
void vector_realloc(struct vectors* vector);
void vector_push_back(struct vectors* vector, double * val);

double norm_1(double* arr, int n, void* data);
double norm_2(double* arr, int n, void* data);
double norm_3(double* arr, int n, void* data);

int dyn_alloc_2(int ***data, int n, int m);
void dyn_free_2(int **data, int n);



int main()
{
    double vec_1[] = {1, 2, 3};
    double vec_2[] = {-1, 0, 0.5};
    double vec_3[] = {5, 7, 3};
    double vec_4[] = {1, 2, 3};

    double p = 2;

    double **matrix;

    if (dyn_alloc_2(&matrix, 3, 3) == -1)
        return 0;

    matrix[0][0] = 1;
    matrix[0][1] = 1;
    matrix[0][2] = 1;
    matrix[1][0] = 1;
    matrix[1][1] = 1;
    matrix[1][2] = 1;
    matrix[2][0] = 1;
    matrix[2][1] = 1;
    matrix[2][2] = 0;

    struct vectors res = max_vector_norms(3, 4, 3, (double *)vec_1, (double *)vec_2, (double *)vec_3, (double *)vec_4, &norm_1, NULL, &norm_2, &p, &norm_3, matrix);

    if (vector_check(res))
    {
        for (int i = 0; i < res.size; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                printf("%.2lf ", res.arr[i][j]);
            }
            printf("\n");
        }

        vector_free(&res);
    }

    dyn_free_2(matrix, 3);
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
    if (!vector_check(res) || vector_count < 1 || norm_count < 1 || n < 1)
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
    double max = arr[0];

    for (int i = 1; i < n; ++i)
    {
        if (arr[i] > max)
            max = arr[i];
    }

    return max;
}

double norm_2(double* arr, int n, void* data)
{
    double p = *((double *)data);
    double accum = 0;

    for (int i = 0; i < n; ++i)
    {
        accum += pow(arr[i], p);
    }
    return pow(accum, 1 / p);
}

double norm_3(double* arr, int n, void* data)
{
    double **matrix = (double**)data;
    double *res = malloc(n * sizeof(double ));
    if (res == NULL)
        return 0;

    matix_vec_prod(matrix, arr, res, n);
    double accum = 0;

    for (int i = 0; i < n; ++i)
    {
        accum += res[i] * arr[i];
    }

    free(res);
    return sqrt(accum);
}

void matix_vec_prod(double **matrix, double *vec, double* res, int n)
{
    for (int i = 0; i < n; ++i)
    {
        double accum = 0;
        for (int j = 0; j < n; ++j)
        {
            accum += matrix[i][j] * vec[j];
        }
        res[i] = accum;
    }
}

int dyn_alloc_2(int ***data, int n, int m) {
    *data = malloc(n * sizeof(int *));
    if (*data == NULL) return -1;

    for (int i = 0; i < n; ++i) {
        (*data)[i] = malloc(m * sizeof(int));
        if ((*data)[i] == NULL) {
            for (int j = 0; j < i; ++j) free((*data)[i]);
            free(*data);
            return -1;
        }
    }

    return 0;
}

void dyn_free_2(int **data, int n) {
    for (int i = 0; i < n; ++i) {
        free(data[i]);
    }

    free(data);
}
