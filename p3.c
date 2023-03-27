#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/time.h>
#include <assert.h>

double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double)t.tv_sec + (double)t.tv_usec/1e6;
}

void Spin(int howlong) {
    double t = GetTime();
    while ((GetTime() - t) < (double)howlong)
	; // do nothing in loop
}

void threadfunc() {
    while (1) {
        sem_wait(&semaphore);
        printf("Hello from da thread!\n");
        sem_post(&semaphore);
        Spin(5);
    }
}

sem_t semaphore;

int main(void) {
    
    // initialize semaphore, only to be used with threads in this process, set value to 1
    sem_init(&semaphore, 0, 1);
    
    pthread_t* mythread;
    
    mythread = (pthread_t*)malloc(sizeof(*mythread));
    
    // start the thread
    printf("Starting thread, semaphore is unlocked.\n");
    pthread_create(mythread, NULL, (void*)threadfunc, NULL);
    
    getchar();
    
    sem_wait(&semaphore);
    printf("Semaphore locked.\n");
    
    // do stuff with whatever is shared between threads
    getchar();
    
    printf("Semaphore unlocked.\n");
    sem_post(&semaphore);
    
    getchar();
    
    return 0;
}