#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "read_ppm.h"
#include "write_ppm.h"

#define MAX_ITER 1000

// Structure to hold the parameters for each thread
typedef struct {
    int start_col, end_col;
    int start_row, end_row;
    float xmin, xmax, ymin, ymax;
    int size;
    int **membership;
    int **visited_counts;
    int *max_count;
    pthread_mutex_t *mutex;
    struct ppm_pixel* image;
} ThreadData;

pthread_barrier_t barrier;

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

void* compute_buddhabrot(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int width = data->size;
    int height = data->size;
    int max_iter = MAX_ITER;

    // Step 1: Determine Mandelbrot membership
    for (int row = data->start_row; row < data->end_row; row++) {
        for (int col = data->start_col; col < data->end_col; col++) {
            float x = data->xmin + (data->xmax - data->xmin) * col / width;
            float y = data->ymin + (data->ymax - data->ymin) * row / height;

            int iter = mandelbrot(x, y, max_iter);
            data->membership[row][col] = (iter == max_iter) ? 1 : 0;
        }
    }

    // Step 2: Compute visited counts for points not in the Mandelbrot set
    for (int row = data->start_row; row < data->end_row; row++) {
        for (int col = data->start_col; col < data->end_col; col++) {
            if (data->membership[row][col] == 1) continue; // Skip Mandelbrot set points

            float x0 = data->xmin + (data->xmax - data->xmin) * col / width;
            float y0 = data->ymin + (data->ymax - data->ymin) * row / height;
            float x = 0, y = 0;

            while (x*x + y*y < 4.0) {
                float xtmp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = xtmp;

                int yrow = round(height * (y - data->ymin) / (data->ymax - data->ymin));
                int xcol = round(width * (x - data->xmin) / (data->xmax - data->xmin));
                if (yrow < 0 || yrow >= height || xcol < 0 || xcol >= width) continue;

                pthread_mutex_lock(data->mutex);
                data->visited_counts[yrow][xcol]++;
                if (data->visited_counts[yrow][xcol] > *data->max_count) {
                    *data->max_count = data->visited_counts[yrow][xcol];
                }
                pthread_mutex_unlock(data->mutex);
            }
        }
    }

    pthread_barrier_wait(&barrier);

    // Step 3: Compute colors
    float gamma = 0.681;
    float factor = 1.0 / gamma;
    for (int row = data->start_row; row < data->end_row; row++) {
        for (int col = data->start_col; col < data->end_col; col++) {
            float value = 0;
            if (data->visited_counts[row][col] > 0) {
                value = log(data->visited_counts[row][col]) / log(*data->max_count);
                value = pow(value, factor);
            }
            data->image[row * width + col].red = value * 255;
            data->image[row * width + col].green = value * 255;
            data->image[row * width + col].blue = value * 255;
        }
    }

    pthread_t thread_id = pthread_self();
    printf("Thread %lu) sub-image block: cols (%d, %d) to rows (%d, %d)\n",
           thread_id, data->start_col, data->end_col, data->start_row, data->end_row);

    return NULL;
}

int main(int argc, char* argv[]) {
    int size = 480;
    float xmin = -2.0;
    float xmax = 0.47;
    float ymin = -1.12;
    float ymax = 1.12;
    int numThreads = 4;

    int opt;
    while ((opt = getopt(argc, argv, ":s:l:r:t:b:p:")) != -1) {
        switch (opt) {
            case 's': size = atoi(optarg); break;
            case 'l': xmin = atof(optarg); break;
            case 'r': xmax = atof(optarg); break;
            case 't': ymax = atof(optarg); break;
            case 'b': ymin = atof(optarg); break;
            case 'p': numThreads = atoi(optarg); break;
            case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
                              "-b <ymin> -t <ymax> -p <numThreads>\n", argv[0]); return 1;
        }
    }

    printf("Generating buddhabrot with size %dx%d\n", size, size);
    printf("  Num threads = %d\n", numThreads);
    printf("  X range = [%.4f, %.4f]\n", xmin, xmax);
    printf("  Y range = [%.4f, %.4f]\n", ymin, ymax);

    struct ppm_pixel* image = (struct ppm_pixel*)malloc(size * size * sizeof(struct ppm_pixel));
    if (!image) {
        fprintf(stderr, "Failed to allocate memory for image\n");
        return 1;
    }

    int **membership = (int**)malloc(size * sizeof(int*));
    int **visited_counts = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        membership[i] = (int*)malloc(size * sizeof(int));
        visited_counts[i] = (int*)malloc(size * sizeof(int));
    }
    int max_count = 0;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, numThreads);

    pthread_t* threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    ThreadData* threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    if (!threads || !threadData) {
        fprintf(stderr, "Failed to allocate memory for threads or thread data\n");
        free(image);
        return 1;
    }

    int rows_per_thread = size / 2;
    int cols_per_thread = size / 2;

    clock_t start_time = clock();

    for (int i = 0; i < numThreads; i++) {
        threadData[i].xmin = xmin;
        threadData[i].xmax = xmax;
        threadData[i].ymin = ymin;
        threadData[i].ymax = ymax;
        threadData[i].size = size;
        threadData[i].membership = membership;
        threadData[i].visited_counts = visited_counts;
        threadData[i].max_count = &max_count;
        threadData[i].mutex = &mutex;
        threadData[i].image = image;
        threadData[i].start_row = (i / 2) * rows_per_thread;
        threadData[i].end_row = (i / 2 + 1) * rows_per_thread;
        threadData[i].start_col = (i % 2) * cols_per_thread;
        threadData[i].end_col = (i % 2 + 1) * cols_per_thread;

        pthread_create(&threads[i], NULL, compute_buddhabrot, (void*)&threadData[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Computed buddhabrot set (%dx%d) in %.6f seconds\n", size, size, time_taken);

    time_t now = time(NULL);
    struct tm* time_info = localtime(&now);
    char filename[100];
    strftime(filename, sizeof(filename), "buddhabrot-%dx%d-%Y%m%d%H%M%S.ppm", time_info);

    write_ppm(filename, image, size, size);
    printf("Writing file: %s\n", filename);
    for (int i = 0; i < size; i++) {
	    free(membership[i]);
	    free(visited_counts[i]);
    }
    free(membership);
    free(visited_counts);
    free(image);
    free(threads);
    free(threadData);
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);

    return 0;
}
