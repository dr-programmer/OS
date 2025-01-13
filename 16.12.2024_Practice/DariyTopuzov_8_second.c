#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 10000
#define ARRAY_CHUNKS 100

pthread_mutex_t mutex;

int *array;

void *sum_array(void *args) {
    if(args == NULL) return NULL;

    int start_index = *(int *)args;
    int chunk_size = ARRAY_SIZE / ARRAY_CHUNKS;
    int size = start_index + chunk_size;

    int result = 0;
    for(int i = start_index; i < size; i++) {
        result += array[i];
    }

    free(args);

    return (void *)(long)result;
}

int main() {
    pthread_mutex_init(&mutex, NULL);

    array = (int *)calloc(ARRAY_SIZE, sizeof(int));

    for(int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i + 1;
    }

    int chunk_size = ARRAY_SIZE / ARRAY_CHUNKS;

    pthread_t threads[ARRAY_CHUNKS] = {0};
    for(int i = 0; i < ARRAY_CHUNKS; i++) {
        int *start_index = (int *)calloc(1, sizeof(int));
        *start_index = i * chunk_size;
        if(pthread_create(threads + i, NULL, sum_array, start_index) != 0) {
            perror("Error creating thread! \n");
            return 1;
        }
    }

    int result = 0;
    for(int i = 0; i < ARRAY_CHUNKS; i++) {
        int temp_result = 0;
        pthread_join(threads[i], (void *)&temp_result);
        result += temp_result;
    }

    printf("Result = %u \n", result);

    free(array);

    pthread_mutex_destroy(&mutex);

    return 0;
}