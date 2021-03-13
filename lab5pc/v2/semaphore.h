
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <pthread.h>

typedef struct {
	int value = 1;
	pthread_mutex mutex;
}Semaphore;


void wait(Semaphore* sem);
void signal(Semaphore* sem);

#endif