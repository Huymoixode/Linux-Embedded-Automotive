#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 5
#define COUNT 10000

int share_count = 0;
pthread_mutex_t mtx;

void * func(void* arg){
    printf("LOCK mtx\n");
   // printf("%lu\n",pthread_self());
    pthread_mutex_lock(&mtx);
    for(int i = 0; i< COUNT;i++){
        share_count++;
    }
    pthread_mutex_unlock(&mtx);
    return NULL;
}

int main(){
    pthread_t thread[NUM_THREADS];
    pthread_mutex_init(&mtx,NULL);  
    printf("%lu\n",pthread_self());
    for(int i=0;i <NUM_THREADS ;i++){
        pthread_create(thread+i,NULL,func,NULL);
    }
     for(int i=0;i <NUM_THREADS ;i++){
        pthread_join(thread[i],NULL);
    }
    pthread_mutex_destroy(&mtx);
    printf("%d ",share_count);
    return 0;
}