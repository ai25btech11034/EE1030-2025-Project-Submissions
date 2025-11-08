#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <math.h>

typedef struct {
    int rows;
    int cols;
    double **data;
} Matrix;

Matrix* create_matrix(int rows, int cols);
void free_matrix(Matrix *m);
Matrix* copy_matrix(Matrix *m);

void matrix_multiply(Matrix *A, Matrix *B, Matrix *result);
void matrix_transpose(Matrix *A, Matrix *result);
void matrix_vector_multiply(Matrix *A, double *v, double *result);
double vector_dot(double *a, double *b, int n);
void vector_normalize(double *v, int n);
double vector_norm(double *v, int n);

#endif