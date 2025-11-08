#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgm_io.h"
#include "svd_compress.h"

void print_usage(const char *prog_name) {
    printf("Usage: %s <input> <output> <k>\n", prog_name);
    printf("  input  - Input image (JPG, PNG, or PGM P5 format)\n");
    printf("  output - Output compressed image (JPG, PNG, or PGM P5 format)\n");
    printf("  k      - Number of singular values to keep (compression rank)\n");
    printf("\nExample: %s input.jpg compressed.jpg 50\n", prog_name);
}

int main(int argc, char *argv[]) {
    printf("=================================\n");
    printf("  Image Compressor using SVD\n");
    printf("  Supports JPG, PNG, PGM formats\n");
    printf("=================================\n\n");
    
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char *input_file = argv[1];
    const char *output_file = argv[2];
    int k = atoi(argv[3]);
    
    if (k <= 0) {
        fprintf(stderr, "Error: k must be a positive integer\n");
        return 1;
    }
    
 
    printf("Reading input image: %s\n", input_file);
    PGMImage *img = read_image(input_file);
    if (!img) {
        fprintf(stderr, "Error: Failed to read input image\n");
        return 1;
    }
    
 
    int max_k = (img->width < img->height) ? img->width : img->height;
    if (k > max_k) {
        printf("Warning: k=%d exceeds image dimensions, using k=%d instead\n", k, max_k);
        k = max_k;
    }
    
 
    PGMImage *compressed = compress_image_svd(img, k);
    if (!compressed) {
        fprintf(stderr, "Error: Compression failed\n");
        free_pgm_image(img);
        return 1;
    }
    
   
    printf("Writing compressed image: %s\n", output_file);
    if (!write_image(output_file, compressed)) {
        fprintf(stderr, "Error: Failed to write output image\n");
        free_pgm_image(img);
        free_pgm_image(compressed);
        return 1;
    }
    
    printf("\nSuccess! Compressed image saved to %s\n", output_file);
    
   
    free_pgm_image(img);
    free_pgm_image(compressed);
    
    return 0;
}