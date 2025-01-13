#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void dump(const char * const filename, int *array, size_t size) {
    if(!filename || !array) return;

    const int file_d = open(filename, O_WRONLY);

    write(file_d, array, sizeof(int) * size);

    close(file_d);
}

void load(const char * const filename, int *array, int size) {
    if(!filename || !array) return;

    const int file_d = open(filename, O_RDONLY);

    read(file_d, array, sizeof(int) * size);

    close(file_d);
}

int main() {
    const int size = 6;
    int arr[] = {'a', 'b', 'c', 'd', 'e', 'f'};

    dump("temp.txt", arr, size);

    int temp[size];
    load("temp.txt", temp, size);
    for(unsigned int i = 0; i < size; i++) {
        printf("%c ", temp[i]);
    }
    printf("\n");

    return 0;
}