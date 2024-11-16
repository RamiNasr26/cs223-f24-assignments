#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "read_ppm.h"
#include "write_ppm.h"

#define MAX_ITER 300  // Reduced max iterations for faster computation

// Global variables
struct ppm_pixel** image;
int** counts;
int maxCount;
pthread_mutex_t countMutex;
pthread_barrier_t barrier;

typedef struct {
    int id;
    int size;
    int numThreads;
    float xmin, xmax, ymin, ymax;
} ThreadData;

void* compute_buddhabrot(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int size = data->size;
    int numThreads = data->numThreads;
    int id = data->id;

    // Determine row and column range for this thread
    int startRow = (id / 2) * (size / 2);
    int endRow = startRow + (size / 2);
    int startCol = (id % 2) * (size / 2);
    int endCol = startCol + (size / 2);

    for (int row = startRow; row < endRow; row++) {
        for (int col = startCol; col < endCol; col++) {
            float real = data->xmin + col * (data->xmax - data->xmin) / size;
            float imag = data->ymin + row * (data->ymax - data->ymin) / size;
            float zr = real, zi = imag;
            int iter;

            for (iter = 0; iter < MAX_ITER; iter++) {
                float zr2 = zr * zr;
                float zi2 = zi * zi;
                if (zr2 + zi2 > 4.0f) break;

                float newZr = zr2 - zi2 + real;
                zi = 2.0f * zr * zi + imag;
                zr = newZr;
            }

            // Color computation
            int count = (iter == MAX_ITER) ? 0 : iter;
            if (count > 0) {
                struct ppm_pixel* pixel = &image[row][col];
                int color = (count * 255) / MAX_ITER;
                pixel->red = color;
                pixel->green = color / 2;
                pixel->blue = color / 4;
            }
        }
    }

    // Synchronize threads
    pthread_barrier_wait(&barrier);

    // Accumulate pixel counts
    for (int row = startRow; row < endRow; row++) {
        for (int col = startCol; col < endCol; col++) {
            int count = counts[row][col];
            struct ppm_pixel* pixel = &image[row][col];
            if (count > 0) {
                int color = (count * 255) / maxCount;
                pixel->red = color;
                pixel->green = color / 2;
                pixel->blue = color / 4;
            } else {
                pixel->red = 0;
                pixel->green = 0;
                pixel->blue = 0;
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int size = 480;
    float xmin = -2.0, xmax = 0.47, ymin = -1.12, ymax = 1.12;
    int numThreads = 4;

    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, ":s:l:r:t:b:p:")) != -1) {
        switch (opt) {
            case 's': size = atoi(optarg); break;
            case 'l': xmin = atof(optarg); break;
            case 'r': xmax = atof(optarg); break;
            case 't': ymax = atof(optarg); break;
            case 'b': ymin = atof(optarg); break;
            case 'p': numThreads = atoi(optarg); break;
            default: fprintf(stderr, "Usage: %s -s <size> -l <xmin> -r <xmax> -b <ymin> -t <ymax> -p <numThreads>\n", argv[0]); exit(EXIT_FAILURE);
        }
    }

    printf("Generating Buddhabrot with size %dx%d\n", size, size);
    printf("  Num threads = %d\n", numThreads);
    printf("  X range = [%.4f, %.4f]\n", xmin, xmax);
    printf("  Y range = [%.4f, %.4f]\n", ymin, ymax);

    // Allocate memory for the image and counts
    image = (struct ppm_pixel**)malloc(size * sizeof(struct ppm_pixel*));
    counts = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        image[i] = (struct ppm_pixel*)malloc(size * sizeof(struct ppm_pixel));
        counts[i] = (int*)calloc(size, sizeof(int));
    }

    pthread_mutex_init(&countMutex, NULL);
    pthread_barrier_init(&barrier, NULL, numThreads);

    clock_t start = clock();

    // Create thread data array and thread handles
    ThreadData* threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    pthread_t* threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));

    for (int i = 0; i < numThreads; i++) {
        threadData[i].id = i;
        threadData[i].size = size;
        threadData[i].numThreads = numThreads;
        threadData[i].xmin = xmin;
        threadData[i].xmax = xmax;
        threadData[i].ymin = ymin;
        threadData[i].ymax = ymax;
        pthread_create(&threads[i], NULL, compute_buddhabrot, &threadData[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end = clock();
    double elapsedTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Computed Buddhabrot set (%dx%d) in %.6f seconds\n", size, size, elapsedTime);

    char filename[64];
    sprintf(filename, "buddhabrot-%d.ppm", size);
    write_ppm(filename, image[0], size, size);
    printf("Writing image file: %s\n", filename);

    // Free allocated memory
    for (int i = 0; i < size; i++) {
        free(image[i]);
        free(counts[i]);
    }
    free(image);
    free(counts);
    free(threads);
    free(threadData);
    pthread_mutex_destroy(&countMutex);
    pthread_barrier_destroy(&barrier);

    return 0;
}

