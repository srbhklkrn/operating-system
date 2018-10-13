/*Write a program that creates 5 pthreads.
 * Each pthread executes a function that involves iterating through a loop 50 times.
 * In each iteration of the loop, a thread increments an int from 0 to 0.9*MAX_INT and then prints the iteration number.
 * Make sure that each of the 5 threads finish the ith iteration of the loop before they can start the (i+1)th iteration
 * (i.e. all threads synchronize/rendezvous towards the end of each iteration).
 * Use a two-phase barrier implemented using POSIX semaphores to enforce the synchronization constraint.*/

#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>

static sem_t mutex;
static sem_t turnstile1;
static sem_t turnstile2;
static sig_atomic_t count = 0;
static const int n = 5;

void* my_thread(void* args) {
    for (int i = 1; i <= 10; ++i) {
        for (int j = 0; j < 0.009 * INT_MAX; ++j) {}

        sem_wait(&mutex);
        	++count;
        	if (count == n) {
        		sem_wait(&turnstile2);
                	sem_post(&turnstile1);
        	}
        sem_post(&mutex);

        sem_wait(&turnstile1);
        sem_post(&turnstile1);

        printf("Thread: %d, Iteration: %d\n", *(int*)args, i);

        sem_wait(&mutex);
	        --count;
        	if (count == 0) {
        		sem_wait(&turnstile1);
                	sem_post(&turnstile2);
        	}
        sem_post(&mutex);

        sem_wait(&turnstile2);
        sem_post(&turnstile2);
    }
    return NULL;
}


int main() {
    pthread_t threads[n];
    int id[n];
    for (int i = 0; i < n; ++i) {
        id[i] = i + 1;
    }
    sem_init(&mutex, 0, 1);
    sem_init(&turnstile1, 0, 0);
    sem_init(&turnstile2, 0, 1);
    for (int i = 0; i < n; ++i) {
        pthread_create(&threads[i], NULL, my_thread, &id[i]);
    }
    for (int i = 0; i < n; ++i) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
