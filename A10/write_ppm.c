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

  fprintf(fp, "P6\n");
  fprintf(fp, "%d %d\n", w, h);
  fprintf(fp, "255\n");

  size_t written = fwrite(pxs, sizeof(struct ppm_pixel), w * h, fp);
  if (written != w * h) {
    printf("Error: unable to write pixel data to '%s'\n", filename);
  }

  fclose(fp);
}

void write_ppm_2d(const char* filename, struct ppm_pixel** pxs, int w, int h) {
  printf("write_ppm_2d is currently unimplemented.\n");
}

