#ifndef LANCZOS_H
#define LANCZOS_H

#include "matrix.h"

typedef struct {
    int k;                  
    double *singular_values; 
    Matrix *U;              
    Matrix *V;             
} SVDResult;

SVDResult* lanczos_svd(Matrix *A, int k, int max_iter);
void free_svd_result(SVDResult *svd);

void compute_tridiagonal_eigenvalues(double *alpha, double *beta, int n, 
                                     double *eigenvalues, double **eigenvectors);

#endif