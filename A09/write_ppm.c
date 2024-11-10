#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

void write_ppm(const char* filename, struct ppm_pixel* pxs, int w, int h) {
  FILE* fp = fopen(filename, "wb");
  if (!fp) {
    printf("Error: unable to open file '%s' for writing\n", filename);
    return;
  }

  // Write the PPM header
  fprintf(fp, "P6\n");
  fprintf(fp, "%d %d\n", w, h);
  fprintf(fp, "255\n");

  // Write the pixel data
  size_t written = fwrite(pxs, sizeof(struct ppm_pixel), w * h, fp);
  if (written != w * h) {
    printf("Error: unable to write pixel data to '%s'\n", filename);
  }

  fclose(fp);
}

void write_ppm_2d(const char* filename, struct ppm_pixel** pxs, int w, int h) {
  // This function is left unimplemented as per your request.
  // If you need it, you can implement it similarly to write_ppm,
  // but by writing each row of pixels individually.
  printf("write_ppm_2d is currently unimplemented.\n");
}

