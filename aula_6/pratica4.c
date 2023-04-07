#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>

sem_t s;

void * th1(void *n){
	int id = *(int*)n;
	while(1){
		sem_wait(&s);
		for(int i=0;i<5;i++){
			printf("thread:%d\n", id);
			fflush(stdout);
			if(i == 4){
				sem_post(&s);
			}
			sleep(1);
		}
	}
}

int main(int argc, char **argv){
	pthread_t thread1, thread2, thread3, thread4;
	int thread_ids[4] = {0, 1, 2, 3};

	sem_init(&s, 0, 2);
	
	if(pthread_create(&thread1, NULL, th1, &thread_ids[0])) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	if(pthread_create(&thread2, NULL, th1, &thread_ids[1])) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	
	if(pthread_create(&thread3, NULL, th1, &thread_ids[2])) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	if(pthread_create(&thread4, NULL, th1, &thread_ids[3])) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}


	if(pthread_join(thread1, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 1;
	}

	return 0;
}
