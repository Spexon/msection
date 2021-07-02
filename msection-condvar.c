// msection-condvar

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


typedef struct {
    int counter;
    pthread_mutex_t mutex;
    pthread_cond_t condvar;
} Condvar; // Unsure of a better variable name


void *doWork(Condvar* condvar);
void doCriticalWork(Condvar* condvar);
void enter(Condvar* condvar);
void leave(Condvar* condvar);


int M = 3;


int main() {

    int N = 10;
    
    Condvar condvar;
    pthread_t tid[N];     // The thread identifier
    pthread_attr_t attr;  // Set of thread attributes

    condvar.counter = 0;
    
    // initialize all pthread functions
    pthread_mutex_init(&condvar.mutex, NULL); 
    pthread_cond_init(&condvar.condvar, NULL);
    pthread_attr_init(&attr);

    for (int i = 0; i < N; i++) {
        pthread_create(&tid[i], &attr, &doWork, &condvar);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }

    // cleanup
    pthread_mutex_destroy(&condvar.mutex);
    pthread_cond_destroy(&condvar.condvar);
    pthread_attr_destroy(&attr);

}

void *doWork(Condvar* condvar) {
   while(true) {
        
        enter(condvar);
        doCriticalWork(condvar); // execute m-section
        leave(condvar);

   }
}

void doCriticalWork(Condvar* condvar) {

    printf("Current thread id = %ld\n", pthread_self()); 
    printf("Number of threads in the m-section = %d\n\n", condvar->counter); // how many threads in the m-section?

    sleep(1);
}


void enter(Condvar* condvar) {

    pthread_mutex_lock(&condvar->mutex); // Enter critical section
    
    while(condvar->counter >= M) {
        
        pthread_cond_wait(&condvar->condvar, &condvar->mutex);
    }
    condvar->counter++;

    // I don't unlock here because our critical section is in 'doCriticalWork'
}


void leave(Condvar* condvar) {

    pthread_mutex_unlock(&condvar->mutex); // leave critical section
    sleep(1);
    condvar->counter--;
    pthread_cond_signal(&condvar->condvar);

}