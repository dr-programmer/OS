#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// Structure to hold thread data
struct ThreadData {
    int start_row;
    int end_row;
    int **A;
    int **B;
    int **C;
    int N;
    int P;
    int M;
};

// Function to allocate a 2D array
int **allocate_matrix(int rows, int cols) {
    int **matrix = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(int));
    }
    return matrix;
}

// Function to free a 2D array
void free_matrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to read a matrix from a file
int **read_matrix(const char *filename, int *rows, int *cols) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char ch;
    char buffer[16];
    int buffer_index = 0;
    *rows = 0;
    *cols = 0;

    while (read(fd, &ch, 1) > 0 && ch != '\n') {
        if (ch == ';') {
            buffer[buffer_index] = '\0';
            *rows = atoi(buffer);
            buffer_index = 0;
        } else {
            buffer[buffer_index++] = ch;
        }
    }
    buffer[buffer_index] = '\0';
    *cols = atoi(buffer);

    int **matrix = allocate_matrix(*rows, *cols);

    int row = 0, col = 0;
    buffer_index = 0;
    while (read(fd, &ch, 1) > 0) {
        if (ch == ';' || ch == '\n') {
            buffer[buffer_index] = '\0';
            matrix[row][col] = atoi(buffer);
            buffer_index = 0;
            if (ch == ';') {
                col++;
            } else if (ch == '\n') {
                col = 0;
                row++;
            }
        } else {
            buffer[buffer_index++] = ch;
        }
    }
    buffer[buffer_index] = '\0';
    matrix[row][col] = atoi(buffer);

    close(fd);
    return matrix;
}

// Function to write a matrix to a file
void write_matrix(const char *filename, int **matrix, int rows, int cols) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char dimension_buffer[32];
    int dim_len = snprintf(dimension_buffer, sizeof(dimension_buffer), "%d;%d\n", rows, cols);
    if (write(fd, dimension_buffer, dim_len) == -1) {
        perror("Error writing dimensions to file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            char num_buffer[16];
            int num_len = snprintf(num_buffer, sizeof(num_buffer), "%d", matrix[i][j]);

            if (write(fd, num_buffer, num_len) == -1) {
                perror("Error writing to file");
                close(fd);
                exit(EXIT_FAILURE);
            }

            if (j < cols - 1) {
                if (write(fd, ";", 1) == -1) {
                    perror("Error writing to file");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
            }
        }

        if (write(fd, "\n", 1) == -1) {
            perror("Error writing to file");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
}

void *thread_matmul(void *args) {
    if(!args) return NULL;

    struct ThreadData data = *(struct ThreadData *)args;
    for(unsigned int row = data.start_row; row < data.end_row; row++) {
        for(unsigned int column = 0; column < data.P; column++) {
            for(unsigned int addition_step = 0; addition_step < data.N; addition_step++) {
                data.C[row][column] += data.A[row][addition_step] * data.B[addition_step][column];
            }
        }
    }

    return args;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <matrix_a.txt> <matrix_b.txt> <matrix_c.txt> <num_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *file_a = argv[1];
    const char *file_b = argv[2];
    const char *file_c = argv[3];
    int num_threads = atoi(argv[4]);

    if (num_threads <= 0) {
        printf("Number of threads must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    int M, N, N1, N2, P;
    int **A = read_matrix(file_a, &M, &N1);
    int **B = read_matrix(file_b, &N2, &P);
    N = N2;

    int **C = allocate_matrix(M, P);

    printf("Read matrices from files.\n");


    //START HERE --------------------------------------------------------------

    if(N1 != N2) {
        printf("Wrong dimensions\n");
        return 1;
    }

    int rows_per_thread = M / num_threads;
    int remainder_rows = M - (rows_per_thread * num_threads);

    pthread_t threads[num_threads];

    struct ThreadData data[num_threads];
    for(unsigned int i = 0; i < num_threads; i++) {
        data[i].A = A;
        data[i].B = B;
        data[i].C = C;
        data[i].start_row = rows_per_thread * i;
        data[i].end_row = data[i].start_row + rows_per_thread + (i+1 == num_threads ? remainder_rows : 0);
        data[i].M = M;
        data[i].N = N;
        data[i].P = P;
        if(pthread_create(threads + i, NULL, thread_matmul, data + i) != 0) {
            printf("pthread_create() error\n");
            return 1;
        }
    }

    for(unsigned int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Calculated multiplication.\n");
    //END HERE --------------------------------------------------------------


    write_matrix(file_c, C, M, P);
    printf("Written result to file.\n");

    free_matrix(A, M);
    free_matrix(B, N);
    free_matrix(C, M);

    return 0;
}
