#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
# include <semaphore.h>

#define TRAVESSA 10

sem_t travessa;
pthread_mutex_t m1;
int np;

int seserve(){
	if(sem_trywait(&travessa)==0){
		return 0;
	}
	else{
		pthread_mutex_unlock(&m1);
		return 1;
	}
}

void encheTravessa(){
	for(int j=0;j<np;j++){
		sem_post(&travessa);
	}
}

void *canibal(void *p){
	int id = (int) p;
	while(1){
		int conseguiu = seserve();
		if(conseguiu == 0){
			printf("Canibal %d servido.\n", id);
			fflush(stdout);
			sleep(1);
		}
		if(conseguiu == 1){
			printf("Canibal %d pediu mais comida.\n", id);
		}
		
	}
}

void *cozinheiro(void *p){
	while(1){
		if(pthread_mutex_trylock(&m1) == 0){
		encheTravessa();
		printf("Cozinheiro serviu a mesa.\n");
		sleep(1);
		}
	}
}

int main(int argc, char **argv){
	pthread_t td, cz;
	long int nc;

	sem_init(&travessa, 0, 0);
	pthread_mutex_init(&m1, NULL);
	
	if(argc < 2){
		printf("%s <número canibais>\n", argv[0]);
		return 0;
	}

	nc = atoi(argv[1]);
	np = atoi(argv[1]);

	pthread_create(&cz, NULL, cozinheiro, NULL);

	for(int i=0; i<nc; i++){
		pthread_create(&td, NULL, canibal, (void*)i);
		pthread_detach(td);
	}

	getchar();

	return 0;
}
