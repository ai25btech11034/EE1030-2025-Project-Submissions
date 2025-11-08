#include "pgm_io.h"
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

int get_file_extension(const char *filename, char *ext, int max_len) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return 0;
    
    strncpy(ext, dot + 1, max_len - 1);
    ext[max_len - 1] = '\0';
    
    // Convert to lowercase
    for (int i = 0; ext[i]; i++) {
        if (ext[i] >= 'A' && ext[i] <= 'Z') {
            ext[i] = ext[i] + 32;
        }
    }
    return 1;
}

PGMImage* create_pgm_image(int width, int height, int max_gray) {
    PGMImage *img = (PGMImage*)malloc(sizeof(PGMImage));
    if (!img) return NULL;
    
    img->width = width;
    img->height = height;
    img->max_gray = max_gray;
    
    img->data = (unsigned char**)malloc(height * sizeof(unsigned char*));
    if (!img->data) {
        free(img);
        return NULL;
    }
    
    for (int i = 0; i < height; i++) {
        img->data[i] = (unsigned char*)calloc(width, sizeof(unsigned char));
        if (!img->data[i]) {
            for (int j = 0; j < i; j++) free(img->data[j]);
            free(img->data);
            free(img);
            return NULL;
        }
    }
    return img;
}

PGMImage* read_pgm_p5(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }
    
    char magic[3];
    if (fscanf(fp, "%2s", magic) != 1 || strcmp(magic, "P5") != 0) {
        fprintf(stderr, "Error: Not a P5 PGM file\n");
        fclose(fp);
        return NULL;
    }
    
    // Skip comments
    int c;
    while ((c = fgetc(fp)) == '#') {
        while (fgetc(fp) != '\n');
    }
    ungetc(c, fp);
    
    int width, height, max_gray;
    if (fscanf(fp, "%d %d %d", &width, &height, &max_gray) != 3) {
        fprintf(stderr, "Error: Invalid PGM header\n");
        fclose(fp);
        return NULL;
    }
    fgetc(fp); // Skip single whitespace after header
    
    PGMImage *img = create_pgm_image(width, height, max_gray);
    if (!img) {
        fclose(fp);
        return NULL;
    }
    
    for (int i = 0; i < height; i++) {
        if (fread(img->data[i], sizeof(unsigned char), width, fp) != width) {
            fprintf(stderr, "Error: Failed to read image data\n");
            free_pgm_image(img);
            fclose(fp);
            return NULL;
        }
    }
    
    fclose(fp);
    printf("Successfully read PGM image: %dx%d, max_gray=%d\n", width, height, max_gray);
    return img;
}

int write_pgm_p5(const char *filename, PGMImage *img) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot create file %s\n", filename);
        return 0;
    }
    
    fprintf(fp, "P5\n%d %d\n%d\n", img->width, img->height, img->max_gray);
    
    for (int i = 0; i < img->height; i++) {
        if (fwrite(img->data[i], sizeof(unsigned char), img->width, fp) != img->width) {
            fprintf(stderr, "Error: Failed to write image data\n");
            fclose(fp);
            return 0;
        }
    }
    
    fclose(fp);
    printf("Successfully wrote PGM image: %s\n", filename);
    return 1;
}

void free_pgm_image(PGMImage *img) {
    if (!img) return;
    
    if (img->data) {
        for (int i = 0; i < img->height; i++) {
            free(img->data[i]);
        }
        free(img->data);
    }
    free(img);
}

PGMImage* read_image(const char *filename) {
    char ext[10];
    if (get_file_extension(filename, ext, sizeof(ext))) {
        if (strcmp(ext, "pgm") == 0) {
            return read_pgm_p5(filename);
        }
    }
    
    // Use stb_image for JPG, PNG, etc.
    int width, height, channels;
    unsigned char *img_data = stbi_load(filename, &width, &height, &channels, 1); // Force grayscale
    
    if (!img_data) {
        fprintf(stderr, "Error: Cannot load image %s\n", filename);
        return NULL;
    }
    
    PGMImage *img = create_pgm_image(width, height, 255);
    if (!img) {
        stbi_image_free(img_data);
        return NULL;
    }
    
    // Copy data
    for (int i = 0; i < height; i++) {
        memcpy(img->data[i], img_data + i * width, width);
    }
    
    stbi_image_free(img_data);
    printf("Successfully read image: %dx%d (converted to grayscale)\n", width, height);
    return img;
}

int write_jpg(const char *filename, PGMImage *img, int quality) {
    // Flatten 2D array to 1D for stb_image_write
    unsigned char *flat_data = (unsigned char*)malloc(img->height * img->width);
    if (!flat_data) return 0;
    
    for (int i = 0; i < img->height; i++) {
        memcpy(flat_data + i * img->width, img->data[i], img->width);
    }
    
    int result = stbi_write_jpg(filename, img->width, img->height, 1, flat_data, quality);
    free(flat_data);
    
    if (result) {
        printf("Successfully wrote JPG image: %s\n", filename);
    } else {
        fprintf(stderr, "Error: Failed to write JPG image\n");
    }
    return result;
}

int write_image(const char *filename, PGMImage *img) {
    char ext[10];
    if (get_file_extension(filename, ext, sizeof(ext))) {
        if (strcmp(ext, "pgm") == 0) {
            return write_pgm_p5(filename, img);
        } else if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) {
            return write_jpg(filename, img, 90); // 90% quality
        } else if (strcmp(ext, "png") == 0) {
            // Flatten data for PNG
            unsigned char *flat_data = (unsigned char*)malloc(img->height * img->width);
            if (!flat_data) return 0;
            
            for (int i = 0; i < img->height; i++) {
                memcpy(flat_data + i * img->width, img->data[i], img->width);
            }
            
            int result = stbi_write_png(filename, img->width, img->height, 1, flat_data, img->width);
            free(flat_data);
            
            if (result) {
                printf("Successfully wrote PNG image: %s\n", filename);
            }
            return result;
        }
    }
    
    // Default to PGM
    return write_pgm_p5(filename, img);
}