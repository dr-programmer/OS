#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT_SIZE 256
#define MAX_ARG_COUNT 128
#define MAX_PIPE_COUNT 128

enum pipe_ops {
    P_READ, 
    P_WRITE
};

int main(int argc, char **argv) {
    while(1) {
        printf("shell> ");
        char input[MAX_INPUT_SIZE];
        fgets(input, MAX_INPUT_SIZE, stdin);
        char *p = strchr(input, '\n');
        if (p != NULL) *p = '\0';

        if(strcmp(input, "exit") == 0) {
            break;
        }

        char *array[MAX_ARG_COUNT];
        unsigned int command_indices[MAX_PIPE_COUNT];
        unsigned int ci_size = 1;
        array[0] = strtok(input, " ");
        command_indices[0] = 0;
        char *token = array[0];

        for (int i = 1; token && i < MAX_ARG_COUNT; i++) {
            char* temp = strtok(NULL, " ");
            if (temp && strcmp(temp, "|") == 0) {
                array[i] = NULL;
                command_indices[ci_size++] = i + 1;
                continue;
            }
            array[i] = temp;
            token = array[i];
        }

        array[MAX_ARG_COUNT - 1] = NULL;

        int pipe_links[MAX_PIPE_COUNT][2];
        pid_t pids[MAX_PIPE_COUNT];

        for (unsigned int i = 0; i < ci_size; i++) {
            if (pipe(pipe_links[i]) != 0) {
                perror("Pipe failed!");
                exit(EXIT_FAILURE);
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed!");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {
                if (i > 0) {
                    if (dup2(pipe_links[i - 1][P_READ], STDIN_FILENO) < 0) {
                        perror("Dup2 read failed!");
                        exit(EXIT_FAILURE);
                    }
                }
                if (i < ci_size - 1) {
                    if (dup2(pipe_links[i][P_WRITE], STDOUT_FILENO) < 0) {
                        perror("Dup2 write failed!");
                        exit(EXIT_FAILURE);
                    }
                }

                for (unsigned int j = 0; j < i; j++) {
                    close(pipe_links[j][P_READ]);
                    close(pipe_links[j][P_WRITE]);
                }

                execvp(array[command_indices[i]], array + command_indices[i]);
                perror("Process failed!");
                return 1;
            }
            else {
                pids[i] = pid;

                if (i > 0) {
                    close(pipe_links[i - 1][P_READ]);
                    close(pipe_links[i - 1][P_WRITE]);
                }
            }
        }

        if (ci_size > 1) {
            close(pipe_links[ci_size - 1][P_READ]);
            close(pipe_links[ci_size - 1][P_WRITE]);
        }

        for (unsigned int i = 0; i < ci_size; i++) {
            int status;
            if (waitpid(pids[i], &status, 0) < 0) {
                perror("Waitpid failed!");
            }
        }
    }

    return 0;
}
