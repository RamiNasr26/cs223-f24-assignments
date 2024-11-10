#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"

#define MAX_ITER 1000

// Structure to hold the parameters for each thread
typedef struct {
    int start_col, end_col;
    int start_row, end_row;
    float xmin, xmax, ymin, ymax;
    int size;
    struct ppm_pixel* image;
} ThreadData;

int mandelbrot(float c_real, float c_imag, int max_iter) {
    float z_real = 0.0, z_imag = 0.0;
    int iter = 0;
    while (z_real*z_real + z_imag*z_imag <= 4.0 && iter < max_iter) {
        float temp = z_real*z_real - z_imag*z_imag + c_real;
        z_imag = 2.0 * z_real * z_imag + c_imag;
        z_real = temp;
        iter++;
    }
    return iter;
}

void* compute_mandelbrot(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int width = data->size;
    int height = data->size;
    int max_iter = MAX_ITER;

    for (int row = data->start_row; row < data->end_row; row++) {
        for (int col = data->start_col; col < data->end_col; col++) {
            float x = data->xmin + (data->xmax - data->xmin) * col / width;
            float y = data->ymin + (data->ymax - data->ymin) * row / height;

            int iter = mandelbrot(x, y, max_iter);

            int color = iter % 256;
            data->image[row * width + col].red = color;
            data->image[row * width + col].green = color;
            data->image[row * width + col].blue = color;
        }
    }

    // Print thread info
    pthread_t thread_id = pthread_self();
    printf("Thread %lu) sub-image block: cols (%d, %d) to rows (%d, %d)\n",
           thread_id, data->start_col, data->end_col, data->start_row, data->end_row);

    return NULL;
}

int main(int argc, char* argv[]) {
    int size = 2000;
    float xmin = -2.0, xmax = 0.47, ymin = -1.12, ymax = 1.12;
    int numThreads = 4;

    int opt;
    while ((opt = getopt(argc, argv, ":s:l:r:t:b:")) != -1) {
        switch (opt) {
            case 's': size = atoi(optarg); break;
            case 'l': xmin = atof(optarg); break;
            case 'r': xmax = atof(optarg); break;
            case 't': ymax = atof(optarg); break;
            case 'b': ymin = atof(optarg); break;
            case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
                              "-b <ymin> -t <ymax> -p <numThreads>\n", argv[0]); break;
        }
    }

    printf("Generating mandelbrot with size %dx%d\n", size, size);
    printf("  Num threads = %d\n", numThreads);
    printf("  X range = [%.4f, %.4f]\n", xmin, xmax);
    printf("  Y range = [%.4f, %.4f]\n", ymin, ymax);

    // Allocate memory for the image
    struct ppm_pixel* image = (struct ppm_pixel*)malloc(size * size * sizeof(struct ppm_pixel));
    if (!image) {
        fprintf(stderr, "Failed to allocate memory for image\n");
        return 1;
    }

    srand(time(0));

    pthread_t* threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    ThreadData* threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    if (!threads || !threadData) {
        fprintf(stderr, "Failed to allocate memory for threads or thread data\n");
        free(image);
        return 1;
    }

    int rows_per_thread = size / 2;
    int cols_per_thread = size / 2;

    // Start time measurement
    clock_t start_time = clock();

    for (int i = 0; i < numThreads; i++) {
        threadData[i].xmin = xmin;
        threadData[i].xmax = xmax;
        threadData[i].ymin = ymin;
        threadData[i].ymax = ymax;
        threadData[i].size = size;
        threadData[i].image = image;

        // Divide the image into quadrants and assign each thread a sub-region
        threadData[i].start_row = (i / 2) * rows_per_thread;
        threadData[i].end_row = (i / 2 + 1) * rows_per_thread;
        threadData[i].start_col = (i % 2) * cols_per_thread;
        threadData[i].end_col = (i % 2 + 1) * cols_per_thread;

        // Create thread
        pthread_create(&threads[i], NULL, compute_mandelbrot, (void*)&threadData[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Computed mandelbrot set (%dx%d) in %.6f seconds\n", size, size, time_taken);

    time_t now = time(NULL);
    struct tm* time_info = localtime(&now);
    char filename[100];
    strftime(filename, sizeof(filename), "mandelbrot-%dx%d-%Y%m%d%H%M%S.ppm", time_info);

    write_ppm(filename, image, size, size);
    printf("Writing file: %s\n", filename);

    free(threads);
    free(threadData);
    free(image);

    return 0;
}

