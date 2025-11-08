#include "svd_compress.h"
#include <stdio.h>
#include <math.h>

Matrix* pgm_to_matrix(PGMImage *img) {
    Matrix *m = create_matrix(img->height, img->width);
    if (!m) return NULL;
    
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            m->data[i][j] = (double)img->data[i][j];
        }
    }
    return m;
}

PGMImage* matrix_to_pgm(Matrix *m, int max_gray) {
    PGMImage *img = create_pgm_image(m->cols, m->rows, max_gray);
    if (!img) return NULL;
    
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            double val = m->data[i][j];
            
            if (val < 0) val = 0;
            if (val > max_gray) val = max_gray;
            
            img->data[i][j] = (unsigned char)(val + 0.5);
        }
    }
    return img;
}

Matrix* reconstruct_from_svd(SVDResult *svd, int k) {
    if (k > svd->k) k = svd->k;
    
    int m = svd->U->rows;
    int n = svd->V->rows;
    
    Matrix *reconstructed = create_matrix(m, n);
    if (!reconstructed) return NULL;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            reconstructed->data[i][j] = 0.0;
            for (int l = 0; l < k; l++) {
                reconstructed->data[i][j] += 
                    svd->U->data[i][l] * svd->singular_values[l] * svd->V->data[j][l];
            }
        }
    }
    
    return reconstructed;
}

PGMImage* compress_image_svd(PGMImage *img, int k) {
    printf("\n=== Starting SVD Compression ===\n");
    printf("Original image size: %dx%d\n", img->width, img->height);
    printf("Rank for compression: k=%d\n", k);
    
    Matrix *img_matrix = pgm_to_matrix(img);
    if (!img_matrix) {
        fprintf(stderr, "Error converting image to matrix\n");
        return NULL;
    }
    
    SVDResult *svd = lanczos_svd(img_matrix, k, k + 10);
    if (!svd) {
        fprintf(stderr, "Error computing SVD\n");
        free_matrix(img_matrix);
        return NULL;
    }
    
    printf("Reconstructing image...\n");
    Matrix *reconstructed = reconstruct_from_svd(svd, k);
    if (!reconstructed) {
        fprintf(stderr, "Error reconstructing image\n");
        free_svd_result(svd);
        free_matrix(img_matrix);
        return NULL;
    }
    
    PGMImage *compressed_img = matrix_to_pgm(reconstructed, img->max_gray);
    
    double error = calculate_error(img_matrix, reconstructed);
    int total_pixels = img->height * img->width;
    double avg_error = error / total_pixels;
    
    double ratio = calculate_compression_ratio(img->height, img->width, k);
    
    printf("\n=== Compression Statistics ===\n");
    printf("Compression ratio: %.2f:1\n", ratio);
    printf("Storage required: %.2f%% of original\n", 100.0 / ratio);
    printf("Total error: %.2f\n", error);
    printf("Average error per pixel: %.4f\n", avg_error);
    printf("Error percentage: %.2f%%\n", (avg_error / img->max_gray) * 100.0);
    
    free_matrix(img_matrix);
    free_matrix(reconstructed);
    free_svd_result(svd);
    
    printf("=== Compression Complete ===\n\n");
    return compressed_img;
}

double calculate_compression_ratio(int m, int n, int k) {
    // Original storage: m * n
    double original = m * n;
    
    // Compressed storage: U(m*k) + Σ(k) + V(n*k)
    double compressed = m * k + k + n * k;
    
    return original / compressed;
}

double calculate_error(Matrix *original, Matrix *compressed) {
    if (original->rows != compressed->rows || original->cols != compressed->cols) {
        fprintf(stderr, "Error: Matrix dimensions don't match\n");
        return -1.0;
    }
    
    double total_error = 0.0;
    for (int i = 0; i < original->rows; i++) {
        for (int j = 0; j < original->cols; j++) {
            double diff = original->data[i][j] - compressed->data[i][j];
            total_error += fabs(diff);
        }
    }
    
    return total_error;
}