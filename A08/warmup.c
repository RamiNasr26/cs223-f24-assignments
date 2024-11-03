#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("%d] A0\n", getpid());
    printf("%d] B0\n", getpid());

    if ((pid = fork()) == 0) {
        printf("%d] B1\n", getpid());
        printf("%d] Bye\n", getpid());
        return 0;
    }

    wait(NULL);

    printf("%d] C0\n", getpid());
    printf("%d] Bye\n", getpid());

    if ((pid = fork()) == 0) {
        printf("%d] C1\n", getpid());
        printf("%d] Bye\n", getpid());
        return 0;
    }

    wait(NULL);
    return 0;
}

