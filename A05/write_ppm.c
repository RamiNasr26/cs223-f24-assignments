/*----------------------------------------------
 * Author: Rami Nasr 
 * Date: 10/5/2024 
 * Description: This file implements the write_ppm function, which writes the pixel data of an image to a 
 * new binary PPM file, including its header information and RGB values in binary format.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

// Implement the flat array version of write_ppm
void write_ppm(const char* filename, struct ppm_pixel* pixels, int w, int h) {
    // Open the file for writing in binary mode
    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing.\n", filename);
        return;
    }

    // Write the PPM header using fprintf
    fprintf(fp, "P6\n");             // Magic number
    fprintf(fp, "%d %d\n", w, h);     // Width and height
    fprintf(fp, "255\n");             // Max color value

    // Write the pixel data using fwrite
    size_t pixel_count = w * h;
    size_t written = fwrite(pixels, sizeof(struct ppm_pixel), pixel_count, fp);
    if (written != pixel_count) {
        fprintf(stderr, "Error: Failed to write pixel data to %s.\n", filename);
        fclose(fp);
        return;
    }

    // Close the file after writing
    fclose(fp);
    printf("Successfully wrote image to %s\n", filename);
}

void write_ppm_2d(const char* filename, struct ppm_pixel** pixels, int w, int h) {

}
