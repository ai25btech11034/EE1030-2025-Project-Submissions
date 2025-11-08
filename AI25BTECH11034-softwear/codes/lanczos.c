#include "lanczos.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

SVDResult* lanczos_svd(Matrix *A, int k, int max_iter) {
    int m = A->rows;
    int n = A->cols;
    
    if (k > n) k = n;
    if (k > m) k = m;
    
    printf("Computing SVD using power iteration method (k=%d)...\n", k);
    
    SVDResult *result = (SVDResult*)malloc(sizeof(SVDResult));
    result->k = k;
    result->singular_values = (double*)malloc(k * sizeof(double));
    result->U = create_matrix(m, k);
    result->V = create_matrix(n, k);
    
    Matrix *AtA = create_matrix(n, n);
    Matrix *At = create_matrix(n, m);
    matrix_transpose(A, At);
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            AtA->data[i][j] = 0.0;
            for (int l = 0; l < m; l++) {
                AtA->data[i][j] += At->data[i][l] * A->data[l][j];
            }
        }
    }
    
    Matrix *A_deflated = copy_matrix(AtA);
    
    for (int sing_idx = 0; sing_idx < k; sing_idx++) {
        double *v = (double*)malloc(n * sizeof(double));
        double *v_new = (double*)malloc(n * sizeof(double));
        
        srand(time(NULL) + sing_idx);
        for (int i = 0; i < n; i++) {
            v[i] = (double)rand() / RAND_MAX - 0.5;
        }
        vector_normalize(v, n);
        
        for (int iter = 0; iter < 100; iter++) {
            for (int i = 0; i < n; i++) {
                v_new[i] = 0.0;
                for (int j = 0; j < n; j++) {
                    v_new[i] += A_deflated->data[i][j] * v[j];
                }
            }
            
            vector_normalize(v_new, n);
            
            double diff = 0.0;
            for (int i = 0; i < n; i++) {
                diff += (v_new[i] - v[i]) * (v_new[i] - v[i]);
            }
            
            memcpy(v, v_new, n * sizeof(double));
            
            if (sqrt(diff) < 1e-6) break;
        }
        
        double eigenvalue = 0.0;
        for (int i = 0; i < n; i++) {
            double sum = 0.0;
            for (int j = 0; j < n; j++) {
                sum += A_deflated->data[i][j] * v[j];
            }
            eigenvalue += v[i] * sum;
        }
        
        result->singular_values[sing_idx] = sqrt(fabs(eigenvalue));
        
        for (int i = 0; i < n; i++) {
            result->V->data[i][sing_idx] = v[i];
        }
        
        double *u = (double*)malloc(m * sizeof(double));
        for (int i = 0; i < m; i++) {
            u[i] = 0.0;
            for (int j = 0; j < n; j++) {
                u[i] += A->data[i][j] * v[j];
            }
            if (result->singular_values[sing_idx] > 1e-10) {
                u[i] /= result->singular_values[sing_idx];
            }
        }
        
        // Store U vector
        for (int i = 0; i < m; i++) {
            result->U->data[i][sing_idx] = u[i];
        }
        
        // Deflate: A_deflated = A_deflated - eigenvalue * v * v^T
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A_deflated->data[i][j] -= eigenvalue * v[i] * v[j];
            }
        }
        
        free(u);
        free(v);
        free(v_new);
    }
    
    printf("SVD computation complete. Top %d singular values:\n", k < 5 ? k : 5);
    for (int i = 0; i < k && i < 5; i++) {
        printf("  σ[%d] = %.4f\n", i, result->singular_values[i]);
    }
    
    // Cleanup
    free_matrix(AtA);
    free_matrix(At);
    free_matrix(A_deflated);
    
    return result;
}

void free_svd_result(SVDResult *svd) {
    if (!svd) return;
    free(svd->singular_values);
    free_matrix(svd->U);
    free_matrix(svd->V);
    free(svd);
}