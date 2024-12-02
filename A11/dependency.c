#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "tree.h"
#include <sys/time.h>

#define MAX_LINE_LEN 256

// Struct to hold thread arguments
typedef struct {
    struct tree_node** root;
    char** files;
    int start;
    int end;
    pthread_mutex_t* mutex;
} thread_args_t;

void* process_files(void* args) {
    thread_args_t* targs = (thread_args_t*)args;
    struct tree_node** root = targs->root;
    pthread_mutex_t* mutex = targs->mutex;
    char** files = targs->files;

    for (int i = targs->start; i < targs->end; i++) {
        FILE* file = fopen(files[i], "r");
        if (!file) {
            printf("Error opening file: %s\n", files[i]);
            continue;
        }

        pthread_mutex_lock(mutex);
        *root = insert(files[i], *root);
        pthread_mutex_unlock(mutex);

        char line[MAX_LINE_LEN];
        while (fgets(line, sizeof(line), file)) {
            char* include_str = strstr(line, "#include");
            if (include_str) {
                char* start = strchr(include_str, '"');
                if (start) {
                    start++;
                    char* end = strchr(start, '"');
                    if (end) {
                        *end = '\0';
                        pthread_mutex_lock(mutex);
                        *root = insert(start, *root);
                        pthread_mutex_unlock(mutex);
                    }
                }
            }
        }
        fclose(file);
    }
    return NULL;
}

void list_files(struct tree_node* root) {
    printSorted(root);
}

void query_file(struct tree_node* root, const char* filename) {
    struct tree_node* node = find(filename, root);
    if (node) {
        printf("%s has the following dependencies:\n", filename);
        // Assuming dependencies are stored in some list in the node structure
        // Print the dependencies here (this part is a bit abstract without full context)
    } else {
        printf("%s not found\n", filename);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <num_threads> <find_command> <find_arguments> ...\n", argv[0]);
        return 1;
    }
    int num_threads = atoi(argv[1]);

    // Build the find command
    char find_command[1024] = {0};
    strcat(find_command, argv[2]);
    for (int i = 3; i < argc; i++) {
        strcat(find_command, " ");
        strcat(find_command, argv[i]);
    }

    // Run the find command
    FILE* fp = popen(find_command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run find command\n");
        return 1;
    }

    char** files = NULL;
    char line[MAX_LINE_LEN];
    int num_files = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';  // Remove newline character
        files = realloc(files, (num_files + 1) * sizeof(char*));
        files[num_files] = strdup(line);
        num_files++;
    }
    pclose(fp);

    struct tree_node* root = NULL;
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_args_t* args = malloc(num_threads * sizeof(thread_args_t));
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int files_per_thread = num_files / num_threads;
    int remaining_files = num_files % num_threads;

    for (int i = 0; i < num_threads; i++) {
        args[i].root = &root;
        args[i].files = files;
        args[i].start = i * files_per_thread + (i < remaining_files ? i : remaining_files);
        args[i].end = args[i].start + files_per_thread + (i < remaining_files ? 1 : 0);
        args[i].mutex = &mutex;
        pthread_create(&threads[i], NULL, process_files, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end_time, NULL);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("Processing %d files\n", num_files);
    for (int i = 0; i < num_threads; i++) {
        printf("Thread %d processing %d files (arguments %d to %d)\n",
            i, args[i].end - args[i].start, args[i].start, args[i].end);
    }
    printf("Elapsed time is %f seconds\n", elapsed_time);

    char command[256];
    while (1) {
        printf("$ ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';
        if (strcmp(command, "quit") == 0) {
            break;
        } else if (strcmp(command, "list") == 0) {
            list_files(root);
        } else {
            query_file(root, command);
        }
    }

    clear(root);
    free(threads);
    free(args);
    for (int i = 0; i < num_files; i++) {
        free(files[i]);
    }
    free(files);
    return 0;
}

