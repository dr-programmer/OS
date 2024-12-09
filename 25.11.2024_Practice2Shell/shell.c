#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv) {

    while(1) {
        printf("shell> ");
        char input[128];
        fgets(input, 128, stdin);
        char *p = strchr(input, '\n');
        if(p != NULL) *p = '\0';
        // we can just say input[strcspn(input, "\n")] = '\0';

        if(strcmp(input, "exit") == 0) {
            break;
        }

        char *array[128];
        array[0] = strtok(input, " ");
        char *token = array[0];
        for(int i = 1; token; i++) {
            array[i] = strtok(NULL, " ");
            token = array[i];
        }

        pid_t pid = fork();
        if(pid == 0) {
            execvp(array[0], array);
            //execlv(input, array);
            perror("Unable to start the process!");
            return 1;
        }
        else {
            int status;
            wait(&status);
        }
    }

    return 0;
}