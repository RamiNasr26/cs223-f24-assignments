#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "read_ppm.h"
#include "write_ppm.h"

#define MAX_THREADS 8
#define PIXEL(row, col, size) ((row) * (size) + (col))
typedef struct {
    int startRow, endRow;
    int startCol, endCol;
    int size;
    float xmin, xmax, ymin, ymax;
    int maxIterations;
    int **membership;
    int **counts;
    int *maxCount;
    pthread_mutex_t *mutex;
    pthread_barrier_t *barrier;
    struct ppm_pixel **image;
} thread_data_t;

// Helper function to compute Mandelbrot membership for a point
int compute_mandelbrot(float x0, float y0, int maxIterations) {
    float x = 0, y = 0, xtmp;
    int iter = 0;
    while (x * x + y * y < 4.0 && iter < maxIterations) {
        xtmp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtmp;
        iter++;
    }
    return iter >= maxIterations ? 1 : 0;
}

// Thread start routine
void *compute_buddhabrot(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    // Step 1: Determine Mandelbrot membership
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = data->startCol; j < data->endCol; j++) {
            float xfrac = (float)j / data->size;
            float yfrac = (float)i / data->size;
            float x0 = data->xmin + xfrac * (data->xmax - data->xmin);
            float y0 = data->ymin + yfrac * (data->ymax - data->ymin);
            data->membership[i][j] = compute_mandelbrot(x0, y0, data->maxIterations);
        }
    }

    // Step 2: Compute visited counts
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = data->startCol; j < data->endCol; j++) {
            if (data->membership[i][j]) continue; // Skip Mandelbrot set points

            float xfrac = (float)j / data->size;
            float yfrac = (float)i / data->size;
            float x0 = data->xmin + xfrac * (data->xmax - data->xmin);
            float y0 = data->ymin + yfrac * (data->ymax - data->ymin);

            float x = 0, y = 0, xtmp;
            while (x * x + y * y < 4.0) {
                xtmp = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = xtmp;

                int xcol = (int)(data->size * (x - data->xmin) / (data->xmax - data->xmin));
                int yrow = (int)(data->size * (y - data->ymin) / (data->ymax - data->ymin));

                if (xcol >= 0 && xcol < data->size && yrow >= 0 && yrow < data->size) {
                    pthread_mutex_lock(data->mutex);
                    data->counts[yrow][xcol]++;
                    if (data->counts[yrow][xcol] > *data->maxCount) {
                        *data->maxCount = data->counts[yrow][xcol];
                    }
                    pthread_mutex_unlock(data->mutex);
                }
            }
        }
    }

    // Wait for all threads to finish steps 1 and 2
    pthread_barrier_wait(data->barrier);

    // Step 3: Compute colors
    float gamma = 0.681;
    float factor = 1.0 / gamma;
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = data->startCol; j < data->endCol; j++) {
            int count = data->counts[i][j];
            float value = count > 0 ? powf(logf(count) / logf(*data->maxCount), factor) : 0.0f;
            data->image[i][j].red = (unsigned char)(value * 255);
            data->image[i][j].green = (unsigned char)(value * 255);
            data->image[i][j].blue = (unsigned char)(value * 255);
        }
    }

    pthread_exit(NULL);
}
int main(int argc, char* argv[]) {
    int size = 480;
    float xmin = -2.0;
    float xmax = 0.47;
    float ymin = -1.12;
    float ymax = 1.12;
    int maxIterations = 1000;
    int numProcesses = 4;

    int opt;
    while ((opt = getopt(argc, argv, ":s:l:r:t:b:p:")) != -1) {
        switch (opt) {
            case 's': size = atoi(optarg); break;
            case 'l': xmin = atof(optarg); break;
            case 'r': xmax = atof(optarg); break;
            case 't': ymax = atof(optarg); break;
            case 'b': ymin = atof(optarg); break;
            case 'p': numProcesses = atoi(optarg); break;
            case '?': 
                printf("usage: %s -s <size> -l <xmin> -r <xmax> "
                       "-b <ymin> -t <ymax> -p <numProcesses>\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    printf("Generating mandelbrot with size %dx%d\n", size, size);
    printf("  Num processes = %d\n", numProcesses);
    printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
    printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

    // Allocate image as a 1D array
    struct ppm_pixel* image = malloc(size * size * sizeof(struct ppm_pixel));
    if (!image) {
        fprintf(stderr, "Error: Unable to allocate memory for the image\n");
        return EXIT_FAILURE;
    }

    // Placeholder for generating the Buddhabrot (Steps 1-3 need implementation)
    // Example: setting all pixels to red
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            struct ppm_pixel* pixel = &image[PIXEL(row, col, size)];
            pixel->red = 255; // Set red color
            pixel->green = 0; // No green
            pixel->blue = 0; // No blue
        }
    }

    // Create the output filename with timestamp
    char filename[256];
    snprintf(filename, sizeof(filename), "buddhabrot-%d-%ld.ppm", size, time(0));

    // Write the image to the file
    write_ppm(filename, image, size, size);
    printf("Generated image written to %s\n", filename);

    // Free allocated memory
    free(image);

    return EXIT_SUCCESS;
}
