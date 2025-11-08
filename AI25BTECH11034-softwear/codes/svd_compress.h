#ifndef SVD_COMPRESS_H
#define SVD_COMPRESS_H

#include "pgm_io.h"
#include "lanczos.h"

Matrix* pgm_to_matrix(PGMImage *img);

PGMImage* matrix_to_pgm(Matrix *m, int max_gray);

PGMImage* compress_image_svd(PGMImage *img, int k);

Matrix* reconstruct_from_svd(SVDResult *svd, int k);

double calculate_compression_ratio(int m, int n, int k);

double calculate_error(Matrix *original, Matrix *compressed);

#endif