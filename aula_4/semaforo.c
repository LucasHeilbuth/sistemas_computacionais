#include <pthread.h>
#include <stdio.h>
#include<semaphore.h>

#define TOTAL 100000
unsigned int sum = 0;
sem_t s;

/* thread */
void *incr(){
	unsigned int i;
	sem_wait(&s);
	
	for(i=0;i<TOTAL;i++){
		sum += 1;
	}
	sem_post(&s);

	return NULL;
}

int main(){

	pthread_t thread1;
	pthread_t thread2;
	sem_init(&s,0,1);

	if(pthread_create(&thread1, NULL, incr, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	
	if(pthread_create(&thread2, NULL, incr, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}


	if(pthread_join(thread1, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}

	if(pthread_join(thread2, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}

	printf("SUM: %d\n", sum);

	return 0;
}

