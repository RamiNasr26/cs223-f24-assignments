/*----------------------------------------------
 * Author: Rami Nasr
 * Date: 10/11/2024
 * Description: This function reads a PPM (P6) image file in binary format. 
 * It parses the image header (magic number, width, height, and max color value) and then reads the pixel data into a 1D array of struct ppm_pixel.
 * It returns a pointer to this array for further use.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

struct ppm_pixel* read_ppm(const char* filename, int* width, int* height) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s for reading.\n", filename);
        return NULL;
    }

    char magic_number[3];
    int max_val;

    if (fscanf(fp, "%2s", magic_number) != 1 || magic_number[0] != 'P' || magic_number[1] != '6') {
        fprintf(stderr, "Error: Invalid PPM format (must start with P6).\n");
        fclose(fp);
        return NULL;
    }

    char ch;
    do {
        ch = fgetc(fp);
        if (ch == '#') {
            while (fgetc(fp) != '\n'); 
        }
    } while (ch == '\n' || ch == '\r' || ch == ' ' || ch == '#'); 

    ungetc(ch, fp);

    if (fscanf(fp, "%d %d", width, height) != 2) {
        fprintf(stderr, "Error: Invalid PPM header (Width, Height).\n");
        fclose(fp);
        return NULL;
    }

    do {
        ch = fgetc(fp);
        if (ch == '#') {
            while (fgetc(fp) != '\n'); 
        }
    } while (ch == '\n' || ch == '\r' || ch == ' ' || ch == '#'); 

    ungetc(ch, fp);

    if (fscanf(fp, "%d", &max_val) != 1) {
        fprintf(stderr, "Error: Invalid PPM header (Max color value).\n");
        fclose(fp);
        return NULL;
    }

    if (max_val < 0 || max_val > 255) {
        fprintf(stderr, "Error: Max color value must be between 0 and 255.\n");
        fclose(fp);
        return NULL;
    }

    fgetc(fp);

    struct ppm_pixel* pixels = (struct ppm_pixel*)malloc((*width) * (*height) * sizeof(struct ppm_pixel));
    if (pixels == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        fclose(fp);
        return NULL;
    }

    size_t read_count = fread(pixels, sizeof(struct ppm_pixel), (*width) * (*height), fp);
    if (read_count != (*width) * (*height)) {
        fprintf(stderr, "Error: Unexpected end of file while reading pixel data.\n");
        free(pixels);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return pixels;
}

void free_ppm(struct ppm_pixel* pixels) {
    if (pixels != NULL) {
        free(pixels);
    }
}

