#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 1024

typedef struct {
    int thread_id;
    int start_index;
    int end_index;
    char **files;
    int num_files;
    char *keyword;
    int match_count;
} ThreadData;

pthread_mutex_t print_mutex;

void* search_files(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    data->match_count = 0;

    for (int i = data->start_index; i < data->end_index; i++) {
        FILE *file = fopen(data->files[i], "r");
        if (!file) {
            pthread_mutex_lock(&print_mutex);
            fprintf(stderr, "Error opening file: %s\n", data->files[i]);
            pthread_mutex_unlock(&print_mutex);
            continue;
        }

        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, data->keyword)) {
                pthread_mutex_lock(&print_mutex);
                printf("%s:%s", data->files[i], line);
                pthread_mutex_unlock(&print_mutex);
                data->match_count++;
            }
        }
        fclose(file);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "usage: ./grep <NumThreads> <Keyword> <Files>\n");
        return 1;
    }

    int num_threads = atoi(argv[1]);
    char *keyword = argv[2];
    char **files = &argv[3];
    int num_files = argc - 3;

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    ThreadData *thread_data = malloc(num_threads * sizeof(ThreadData));
    if (!threads || !thread_data) {
        fprintf(stderr, "Memory allocation failed\n");
        free(threads);
        free(thread_data);
        return 1;
    }

    pthread_mutex_init(&print_mutex, NULL);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    int files_per_thread = num_files / num_threads;
    int remaining_files = num_files % num_threads;

    int start_index = 0;
    for (int i = 0; i < num_threads; i++) {
        int end_index = start_index + files_per_thread + (i < remaining_files ? 1 : 0);

        thread_data[i].thread_id = i;
        thread_data[i].start_index = start_index;
        thread_data[i].end_index = end_index;
        thread_data[i].files = files;
        thread_data[i].num_files = num_files;
        thread_data[i].keyword = keyword;

        printf("Thread %d searching %d files (%d to %d)\n", i, end_index - start_index, start_index + 1, end_index);

        pthread_create(&threads[i], NULL, search_files, &thread_data[i]);
        start_index = end_index;
    }

    int total_matches = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d found %d lines containing keyword: %s\n",
               thread_data[i].thread_id, thread_data[i].match_count, keyword);
        total_matches += thread_data[i].match_count;
    }

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Total occurrences: %d\n", total_matches);
    printf("Elapsed time is %.6f seconds\n", elapsed);

    pthread_mutex_destroy(&print_mutex);
    free(threads);
    free(thread_data);
    return 0;
}

