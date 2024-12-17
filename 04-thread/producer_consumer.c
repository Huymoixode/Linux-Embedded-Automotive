#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_THREADS 5
#define BUFFER_SIZE 5
#define NUM_ITEM 10

int count = 0;
int in =0, out=0;
//Global Variable
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
int buffer[BUFFER_SIZE];

void* producer(void *arg){
    for(int i=0; i<NUM_ITEM; i++){
        pthread_mutex_lock(&mtx);
        while(count == BUFFER_SIZE){
            printf("Producer full !!\n");
            pthread_mutex_unlock(&mtx);
            sleep(1);

        }
        buffer[in] = rand()%10;
        count++;
        printf("Producer: %d\n",buffer[in]);
        in = (in+1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mtx);
    }
}

void* consumer(void *arg){
    for(int i =0 ;i<NUM_ITEM;i++){
        pthread_mutex_lock(&mtx);
        while(count == 0){
           printf("Consumer empty !!\n");
           pthread_mutex_unlock(&mtx);
           sleep(1);

        }
        printf("Consumer: \t%d\n",buffer[out]);
        count--;
        out = (out+1) %BUFFER_SIZE;
        pthread_mutex_unlock(&mtx);
    }
}
int main(){
    pthread_t thread1;
    pthread_t thread2;
    
    pthread_create(&thread1,NULL,producer,NULL);


    pthread_create(&thread2,NULL,consumer,NULL);


    pthread_join(thread1,NULL);


    pthread_join(thread2,NULL);
    printf("done !!!\n");
    return 0;
}