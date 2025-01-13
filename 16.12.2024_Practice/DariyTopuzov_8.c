#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond;

unsigned long long bank_account = 10000;

#define TRANSACTION_COUNT 50000

void *deposit(void *amount) {
    if(amount == NULL) return NULL;

    for(unsigned int i = 0; i < TRANSACTION_COUNT; i++) {
        pthread_mutex_lock(&mutex);
        pthread_cond_signal(&cond);
        bank_account += *(int *)amount;
        pthread_mutex_unlock(&mutex);
    }

    return amount;
}
void *withdraw(void *amount) {
    if(amount == NULL) return NULL;

    for(unsigned int i = 0; i < TRANSACTION_COUNT / 2; i++) {
        pthread_mutex_lock(&mutex);
        if(*(int *)amount >= bank_account) {
            pthread_cond_wait(&cond, &mutex);
        }
        else bank_account -= *(int *)amount;
        pthread_mutex_unlock(&mutex);
    }

    return (void *)(long)(bank_account != 0 ? 1 : 2);
}

int main() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t thread_deposit, thread_withdraw;

    int amount = 30;

    if(pthread_create(&thread_deposit, NULL, deposit, &amount) != 0) {
        perror("Deposit thread error!\n");
        return 1;
    }
    if(pthread_create(&thread_withdraw, NULL, withdraw, &amount) != 0) {
        perror("Withdraw thread error!\n");
        return 1;
    }

    pthread_join(thread_deposit, NULL);
    int result_withdraw;
    pthread_join(thread_withdraw, (void *)&result_withdraw);

    printf("Bank account: %llu \n", bank_account);

    pthread_mutex_destroy(&mutex);

    return 0;
}