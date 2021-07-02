// msection-sem

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

// Forward declaration
void *doWork(sem_t* sem);
void doCriticalWork(sem_t* sem);
void enter(sem_t* sem);
void leave(sem_t* sem);


int M = 3;


int main() {

    int N = 10;
    sem_t sem;
    pthread_t tid[N];   // The thread identifier
    pthread_attr_t attr; // Set of thread attributes

    sem_init(&sem, 0, M); // This semaphore only allows a max of 3 threads to run at a time in the critical section
    pthread_attr_init(&attr);

    for (int i = 0; i < N; i++) {
        pthread_create(&tid[i], &attr, doWork, &sem);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }

    // cleanup
    sem_destroy(&sem);
    pthread_attr_destroy(&attr);
}


void *doWork(sem_t* sem) {
   while(true) {
        enter(sem);
        doCriticalWork(sem); // execute m-section
        leave(sem);
   }
}


void doCriticalWork(sem_t* sem) {

    int sval;
    sem_getvalue(sem, &sval);

    printf("Current thread id = %ld\n", pthread_self()); 
    printf("Number of threads in the m-section = %d\n", 3 - sval); // how many threads in the m-section?

    usleep(1000000);
}


void enter(sem_t* sem) { // We want at most 3 threads running here at a time, we use a semaphore to check

    sem_wait(sem);

}


void leave(sem_t *sem) {

    sem_post(sem); // To release or signal a semaphore
    sleep(1);      // semaphore takes the first avaliable thread, therefore we sleep 1
}