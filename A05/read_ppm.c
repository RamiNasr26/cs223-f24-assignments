/*----------------------------------------------
 * Author:Rami Nasr 
 * Date: 10/5/2024
 * Description: This file implements the read_ppm function, which reads a binary PPM image file
 * and stores its pixel data in a dynamically allocated array.
 * It also extracts and returns the image's width, height, and RGB values.
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

    // Read and verify the magic number (must be "P6")
    if (fscanf(fp, "%2s", magic_number) != 1 || magic_number[0] != 'P' || magic_number[1] != '6') {
        fprintf(stderr, "Error: Invalid PPM format (must start with P6).\n");
        fclose(fp);
        return NULL;
    }
    printf("Magic number: %s\n", magic_number);

    // Skip any comments or whitespace until we reach width and height
    char ch;
    do {
        ch = fgetc(fp);
        // Skip comment lines starting with '#'
        if (ch == '#') {
            while (fgetc(fp) != '\n'); // Skip to the end of the comment line
        }
    } while (ch == '\n' || ch == '\r' || ch == ' ' || ch == '#'); // Skip additional newlines or spaces

    // Put back the non-whitespace character for fscanf
    ungetc(ch, fp);

    // Read width and height using fscanf
    if (fscanf(fp, "%d %d", width, height) != 2) {
        fprintf(stderr, "Error: Invalid PPM header (Width, Height).\n");
        fclose(fp);
        return NULL;
    }
    printf("Width: %d, Height: %d\n", *width, *height);

    // Skip any comments or whitespace until we reach the max color value
    do {
        ch = fgetc(fp);
        // Skip comment lines starting with '#'
        if (ch == '#') {
            while (fgetc(fp) != '\n'); // Skip to the end of the comment line
        }
    } while (ch == '\n' || ch == '\r' || ch == ' ' || ch == '#'); // Skip additional newlines or spaces

    // Put back the non-whitespace character for fscanf
    ungetc(ch, fp);

    // Read max color value using fscanf
    if (fscanf(fp, "%d", &max_val) != 1) {
        fprintf(stderr, "Error: Invalid PPM header (Max color value).\n");
        fclose(fp);
        return NULL;
    }
    printf("Max Color Value: %d\n", max_val);

    // Ensure max color value is within acceptable range (0-255)
    if (max_val >= 256) {
        fprintf(stderr, "Error: Max color value must be less than 256.\n");
        fclose(fp);
        return NULL;
    }

    // Skip the single whitespace character after max color value
    fgetc(fp);

    // Allocate memory for the pixel array
    struct ppm_pixel* pixels = (struct ppm_pixel*)malloc(sizeof(struct ppm_pixel) * (*width) * (*height));
    if (pixels == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        fclose(fp);
        return NULL;
    }

    // Read pixel data into the allocated array
    size_t read_count = fread(pixels, sizeof(struct ppm_pixel), (*width) * (*height), fp);
    if (read_count != (*width) * (*height)) {
        fprintf(stderr, "Error: Unexpected end of file while reading pixel data.\n");
        free(pixels);
        fclose(fp);
        return NULL;
    }

    // Close file and return the pixel array
    fclose(fp);
    return pixels;
}

