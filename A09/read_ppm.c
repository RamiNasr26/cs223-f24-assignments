#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) {
  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    printf("Error: unable to open file '%s'\n", filename);
    return NULL;
  }

  char format[3];
  fscanf(fp, "%2s", format);
  if (strcmp(format, "P6") != 0) {
    printf("Error: unsupported file format '%s'\n", format);
    fclose(fp);
    return NULL;
  }

  // Read width, height, and maximum color value
  fscanf(fp, "%d %d", w, h);
  int max_color;
  fscanf(fp, "%d", &max_color);
  fgetc(fp); // Consume the newline after max_color

  // Allocate memory for the pixel data
  struct ppm_pixel* pixels = (struct ppm_pixel*)malloc((*w) * (*h) * sizeof(struct ppm_pixel));
  if (!pixels) {
    printf("Error: unable to allocate memory\n");
    fclose(fp);
    return NULL;
  }

  // Read pixel data
  size_t read = fread(pixels, sizeof(struct ppm_pixel), (*w) * (*h), fp);
  if (read != (*w) * (*h)) {
    printf("Error: unexpected end of file\n");
    free(pixels);
    fclose(fp);
    return NULL;
  }

  fclose(fp);
  return pixels;
}

