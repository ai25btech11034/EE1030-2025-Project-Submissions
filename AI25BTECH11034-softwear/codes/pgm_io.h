#ifndef PGM_IO_H
#define PGM_IO_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int width;
    int height;
    int max_gray;
    unsigned char **data;
} PGMImage;

PGMImage* read_pgm_p5(const char *filename);
PGMImage* read_image(const char *filename);  
int write_pgm_p5(const char *filename, PGMImage *img);
int write_jpg(const char *filename, PGMImage *img, int quality);
int write_image(const char *filename, PGMImage *img);  
void free_pgm_image(PGMImage *img);
PGMImage* create_pgm_image(int width, int height, int max_gray);

#endif