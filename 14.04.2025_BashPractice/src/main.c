#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc < 3) return 1;
    if (argc > 3) return 1;

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int c = a - b;

    if (c < 0) return 1;
    else return 0;
}