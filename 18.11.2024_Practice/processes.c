#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

void fork_test() {
    pid_t pid = fork();

    if(pid == 0) {
        printf("Child process: PID = %d\n", getpid());
    }
    else if(pid > 0) {
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
    }
    else {
        perror("fork failed");
    }
}

void exec_test() {
    pid_t pid = fork();

    if(pid == 0) {
        printf("Before exec()\n");
        execl("/bin/ls", "ls", "-l", NULL);
        perror("exec failed"); // Only runs if execl fails
    }
    else if(pid > 0) {
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
        wait(NULL);
    }
    else {
        perror("fork failed");
    }
}

int main() {
    fork_test();

    exec_test();


    return 0;
}