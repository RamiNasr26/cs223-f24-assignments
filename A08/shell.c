#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <limits.h>

#define MAX_ARGS 64
#define RAINBOW_COLORS 6

const char* colors[] = {
    "\001\033[31m\002",
    "\001\033[33m\002",
    "\001\033[32m\002",
    "\001\033[36m\002",
    "\001\033[34m\002",
    "\001\033[35m\002",
    "\001\033[0m\002"
};

char* get_prompt() {
    static char cwd[PATH_MAX];
    static char prompt[PATH_MAX + 100];
    static int color_index = 0;

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "???");
    }

    snprintf(prompt, sizeof(prompt), "%s%s%s <3 -----> ", 
             colors[color_index], cwd, colors[6]);
    
    color_index = (color_index + 1) % RAINBOW_COLORS;

    return prompt;
}

char** split_command(char* line, int* arg_count) {
    char** args = malloc(MAX_ARGS * sizeof(char*));
    char* token;
    *arg_count = 0;

    token = strtok(line, " \t\n");
    while (token != NULL && *arg_count < MAX_ARGS - 1) {
        args[*arg_count] = strdup(token);
        (*arg_count)++;
        token = strtok(NULL, " \t\n");
    }
    args[*arg_count] = NULL;
    
    return args;
}

void free_args(char** args, int arg_count) {
    for (int i = 0; i < arg_count; i++) {
        free(args[i]);
    }
    free(args);
}

int main() {
    char* line;
    char** args;
    int status, arg_count;
    pid_t pid;

    printf("\033[35m<3 Rainbow Shell <3\033[0m\n");
    printf("------------------\n\n");

    while (1) {
        line = readline(get_prompt());
        
        if (line == NULL) {
            printf("\nGoodbye!\n");
            break;
        }

        if (line[0] == '\0') {
            free(line);
            continue;
        }

        add_history(line);
        args = split_command(line, &arg_count);

        if (strcmp(args[0], "exit") == 0) {
            printf("Goodbye!\n");
            free_args(args, arg_count);
            free(line);
            break;
        }

        pid = fork();
        
        if (pid == 0) {
            execvp(args[0], args);
            fprintf(stderr, "Error: %s: %s\n", args[0], strerror(errno));
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            fprintf(stderr, "Fork failed: %s\n", strerror(errno));
        } else {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Command exited with status %d\n", WEXITSTATUS(status));
            }
        }

        free_args(args, arg_count);
        free(line);
    }

    clear_history();
    
    return 0;
}

