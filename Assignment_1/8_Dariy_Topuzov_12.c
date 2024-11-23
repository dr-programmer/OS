#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#define CALC_OFFSET(FILE_SIZE, CHAR_COUNT) ((FILE_SIZE) - (sizeof(char) * (CHAR_COUNT)))
#define SEEK(F_ID, OFFSET, WHENCE, STOP) if((OFFSET) >= 0) lseek(F_ID, OFFSET, WHENCE); \
                                    else STOP = 1;
#define SMART_PRINT(VAR, SIZE) flag = 0; \
                            flag = write(STDOUT_FILENO, &VAR, SIZE); \
                            if(flag < 0) { \
                                fprintf(stderr, "./tail: error writing 'standard output': No space left on device"); \
                                break; \
                            }

int main(int argc, char* argv[]) {

    const unsigned int lines_to_read = 10;
    int flag = 0;

    if(argc <= 1) {
        fprintf(stderr, "At least 1 parameter should be provided\n");
        return 1;
    }

    for(int i = 1; i < argc; i++) {
        int f_id = open(argv[i], O_RDONLY);
        if(f_id < 0) {
            fprintf(stderr, "./tail: cannot open '%s' for reading: No such file or directory\n", argv[i]);
            continue;
        }

        int file_size = lseek(f_id, 0, SEEK_END);
        if(file_size < 0) {
            fprintf(stderr, "Error seeking file %s \n", argv[i]);
            close(f_id);
            continue;
        }

        char temp_str[128];
        sprintf(temp_str, "\n==> %s <==\n", argv[i]);
        SMART_PRINT(temp_str, sizeof(char) * strlen(temp_str));

        unsigned short stop = 0;
        SEEK(f_id, CALC_OFFSET(file_size, 1), SEEK_SET, stop);
        if(stop) {
            close(f_id);
            continue;
        }

        char sym;
        int c = 1;
        for(int j = 0; j < lines_to_read && read(f_id, &sym, sizeof(char)) > 0; j = sym == '\n' ? j+1 : j, c++) {
            int offset = CALC_OFFSET(file_size, c+1);
            SEEK(f_id, offset, SEEK_SET, stop);
            if(stop) break;
        }
        if(!stop) c-=2;

        lseek(f_id, CALC_OFFSET(file_size, c), SEEK_SET);
        while(read(f_id, &sym, sizeof(char))) {
            SMART_PRINT(sym, sizeof(char));
        }
        sym = '\n';
        SMART_PRINT(sym, sizeof(char));

        if(close(f_id)) {
            fprintf(stderr, "./tail: error reading '%s': Input/output error\n", argv[i]);
        }
    }

    return 0;
}
