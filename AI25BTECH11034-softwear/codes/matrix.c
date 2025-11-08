#include "matrix.h"
#include <string.h>

Matrix* create_matrix(int rows, int cols) {
    Matrix *m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) return NULL;
    
    m->rows = rows;
    m->cols = cols;
    m->data = (double**)malloc(rows * sizeof(double*));
    if (!m->data) {
        free(m);
        return NULL;
    }
    
    for (int i = 0; i < rows; i++) {
        m->data[i] = (double*)calloc(cols, sizeof(double));
        if (!m->data[i]) {
            for (int j = 0; j < i; j++) free(m->data[j]);
            free(m->data);
            free(m);
            return NULL;
        }
    }
    return m;
}

void free_matrix(Matrix *m) {
    if (!m) return;
    if (m->data) {
        for (int i = 0; i < m->rows; i++) {
            free(m->data[i]);
        }
        free(m->data);
    }
    free(m);
}

Matrix* copy_matrix(Matrix *m) {
    Matrix *copy = create_matrix(m->rows, m->cols);
    if (!copy) return NULL;
    
    for (int i = 0; i < m->rows; i++) {
        memcpy(copy->data[i], m->data[i], m->cols * sizeof(double));
    }
    return copy;
}

void matrix_multiply(Matrix *A, Matrix *B, Matrix *result) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < B->cols; j++) {
            result->data[i][j] = 0.0;
            for (int k = 0; k < A->cols; k++) {
                result->data[i][j] += A->data[i][k] * B->data[k][j];
            }
        }
    }
}

void matrix_transpose(Matrix *A, Matrix *result) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            result->data[j][i] = A->data[i][j];
        }
    }
}

void matrix_vector_multiply(Matrix *A, double *v, double *result) {
    for (int i = 0; i < A->rows; i++) {
        result[i] = 0.0;
        for (int j = 0; j < A->cols; j++) {
            result[i] += A->data[i][j] * v[j];
        }
    }
}

double vector_dot(double *a, double *b, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

double vector_norm(double *v, int n) {
    return sqrt(vector_dot(v, v, n));
}

void vector_normalize(double *v, int n) {
    double norm = vector_norm(v, n);
    if (norm > 1e-10) {
        for (int i = 0; i < n; i++) {
            v[i] /= norm;
        }
    }
}