#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Error: please provide a file to open (exactly 1 argument) \n");
        return -1;
    }

    int file_descriptor = open(argv[1], O_RDONLY);

    printf("fd = %d \n", file_descriptor);

    char buf;

    int read_symbols = 0;
    while(read_symbols = read(file_descriptor, &buf, sizeof(char)) != 0) {

        if(read_symbols == -1) {
            perror("Error: read(...)");
            close(file_descriptor);
            return -1;
        }

        putchar(buf);
        write(STDOUT_FILENO, &buf, read_symbols * sizeof(char));
        //fflush(stdout); // This flushes stdout (so no buffering)
    }

    close(file_descriptor);
    

    return 0;
}