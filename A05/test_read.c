/*----------------------------------------------
 * Author: Rami Nasr
 * Date: 10/5/2024
 * Description: This file contains a test program that uses the read_ppm function to read a PPM file
 * and prints its pixel values to verify correct parsing and reading of the image.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

int main() {
    int w, h;
    struct ppm_pixel* pixels = read_ppm("feep-raw.ppm", &w, &h);

    // Check if the file was read successfully
    if (pixels == NULL) {
        fprintf(stderr, "Error: Failed to read PPM file.\n");
        return 1;  // Exit with an error code
    }

    // Print the width and height of the image
    printf("Testing file feep-raw.ppm: %d %d\n", w, h);

    // Iterate over the pixel array and print the pixels in a grid format
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            // Calculate the index of the pixel in the 1D array
            struct ppm_pixel* pixel = &pixels[i * w + j];
            printf("(%d,%d,%d) ", pixel->red, pixel->green, pixel->blue);
        }
        printf("\n");  // Newline after each row
    }

    // Free the allocated memory
    free(pixels);

    return 0;  // Exit successfully
}

