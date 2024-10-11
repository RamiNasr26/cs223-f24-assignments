/*----------------------------------------------
 * Author: Rami Nasr
 * Date: 10/11/2024
 * Description: This function writes pixel data to a PPM file. 
 * It outputs the PPM header (image dimensions and color information) and then writes the pixel array (image data) in binary format to the specified file. 
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

void write_ppm(const char* filename, struct ppm_pixel* pixels, int width, int height) {
    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing.\n", filename);
        return;
    }

    fprintf(fp, "P6\n%d %d\n255\n", width, height);

    fwrite(pixels, sizeof(struct ppm_pixel), width * height, fp);

    fclose(fp);
}
void write_ppm_2d(const char* filename, struct ppm_pixel** pixels, int w, int h) {

}
