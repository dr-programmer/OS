#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <errno.h>

typedef struct
{
    char data;
    unsigned char nextElementAddress;
} block;

int main(int argc, char* argv[]){
    if(argc != 2) {
        fprintf(stderr, "Exactly one parameter is necessary! \n");
        return 1;
    }

    int f_id = open(argv[1], O_RDONLY);
    if(f_id < 0) {
        fprintf(stderr, "%s: No such file\n", argv[1]);
        return 2;
    }

    do {
        block data;
        read(f_id, &data, sizeof(block));
        // putchar(data.data);
        write(STDOUT_FILENO, &data.data, sizeof(data.data));
        if(data.nextElementAddress == 0) break;
        lseek(f_id, data.nextElementAddress, SEEK_SET);
    } while(1);

    fflush(stdout);

    close(f_id);

    return 0;
}