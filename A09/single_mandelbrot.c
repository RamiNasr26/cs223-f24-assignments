#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "read_ppm.h"   
#include "write_ppm.h"

int main(int argc, char* argv[]) {
  int size = 2000;
  float xmin = -2.0;
  float xmax = 0.47;
  float ymin = -1.12;
  float ymax = 1.12;
  int maxIterations = 1000;

  // Parse command-line arguments
  int opt;
  while ((opt = getopt(argc, argv, ":s:l:r:t:b:")) != -1) {
    switch (opt) {
      case 's': size = atoi(optarg); break;
      case 'l': xmin = atof(optarg); break;
      case 'r': xmax = atof(optarg); break;
      case 't': ymax = atof(optarg); break;
      case 'b': ymin = atof(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> -b <ymin> -t <ymax>\n", argv[0]); return -1;
    }
  }
  
  printf("Generating mandelbrot with size %dx%d\n", size, size);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // Allocate memory for the pixel array
  struct ppm_pixel *pixels = malloc(size * size * sizeof(struct ppm_pixel));
  if (!pixels) {
    fprintf(stderr, "Failed to allocate memory for pixels\n");
    return -1;
  }

  // Generate a random color palette
  srand(time(0));
  struct ppm_pixel *colors = malloc(maxIterations * sizeof(struct ppm_pixel));
  if (!colors) {
    fprintf(stderr, "Failed to allocate memory for colors\n");
    free(pixels);
    return -1;
  }
  for (int i = 0; i < maxIterations; i++) {
    colors[i].red = rand() % 256;
    colors[i].green = rand() % 256;
    colors[i].blue = rand() % 256;
  }

  // Measure the time to compute the Mandelbrot set
  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);

  // Compute the Mandelbrot set
  for (int j = 0; j < size; j++) {
    for (int i = 0; i < size; i++) {
      float x0 = xmin + i * (xmax - xmin) / (size - 1);
      float y0 = ymin + j * (ymax - ymin) / (size - 1);
      float x = 0.0;
      float y = 0.0;
      int iter = 0;

      while (x*x + y*y <= 4 && iter < maxIterations) {
        float xtemp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtemp;
        iter++;
      }

      if (iter < maxIterations) {
        pixels[j * size + i] = colors[iter];
      } else {
        pixels[j * size + i].red = 0;
        pixels[j * size + i].green = 0;
        pixels[j * size + i].blue = 0;
      }
    }
  }

  gettimeofday(&tend, NULL);
  double elapsed = (tend.tv_sec - tstart.tv_sec) + (tend.tv_usec - tstart.tv_usec) / 1.0e6;
  printf("Computed mandelbrot set (%dx%d) in %f seconds\n", size, size, elapsed);

  // Generate output filename with timestamp
  char filename[64];
  snprintf(filename, sizeof(filename), "mandelbrot-%d-%ld.ppm", size, time(0));

  write_ppm(filename, pixels, size, size);
  printf("Writing file: %s\n", filename);

  free(pixels);
  free(colors);

  return 0;
}

