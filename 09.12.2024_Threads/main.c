#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t count_mutex;

void *increment_count(void *args) {
    if(args == NULL) return NULL;

    for(unsigned long i = 0; i < 1000000; i++) {
        // pthread_mutex_lock(&count_mutex);
        ++(*(int *)args);
        // pthread_mutex_unlock(&count_mutex);
    }
    printf("Value = %d \n", *((int *)args));
    return args;
}

int main() {
    pthread_t thread1;
    pthread_mutex_init(&count_mutex, NULL);

    int count = 0;
    void *args = (&count);

    if(pthread_create(&thread1, NULL, increment_count, args) != 0) {
        printf("pthread_create() error\n");
        return 1;
    }

    // increment_count(&count);

    pthread_join(thread1, NULL);

    increment_count(&count);

    pthread_mutex_destroy(&count_mutex);

    return 0;
}