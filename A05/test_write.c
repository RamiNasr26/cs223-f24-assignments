/* -------------------------------------------------------------
 * Author: Rami Nasr
 * Date: 10/5/2024
 * Description: This file contains a test program that reads a PPM file, writes its contents to a
 * new PPM file using write_ppm, and then reads and prints the contents of the new file to ensure 
 * that the image data was correctly saved.
 * -------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
    int w, h;

    // Step 1: Read the original PPM file (feep-raw.ppm)
    struct ppm_pixel* pixels = read_ppm("feep-raw.ppm", &w, &h);
    if (pixels == NULL) {
        fprintf(stderr, "Error: Failed to read the original PPM file.\n");
        return 1;
    }

    // Step 2: Write the pixel data to a new PPM file (test.ppm)
    write_ppm("test.ppm", pixels, w, h);

    // Step 3: Read the newly written PPM file (test.ppm) to verify the contents
    struct ppm_pixel* test_pixels = read_ppm("test.ppm", &w, &h);
    if (test_pixels == NULL) {
        fprintf(stderr, "Error: Failed to read the newly written PPM file.\n");
        free(pixels);
        return 1;
    }

    // Step 4: Print the contents of the new PPM file (test.ppm) to verify
    printf("Testing file test.ppm: %d %d\n", w, h);
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            struct ppm_pixel* pixel = &test_pixels[i * w + j];
            printf("(%d,%d,%d) ", pixel->red, pixel->green, pixel->blue);
        }
        printf("\n");
    }

    // Free the allocated memory
    free(pixels);
    free(test_pixels);

    return 0;
}

