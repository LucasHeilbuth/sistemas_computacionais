#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

pthread_mutex_t m1;

struct philo_data{
	int id;
	pthread_mutex_t* fork_right;
	pthread_mutex_t* fork_left;
};

enum philo_state {THINKING, EATING, HUNGRY};

void* philosopher(void *p){
	struct philo_data *dt = (struct philo_data*)p;
	enum philo_state state = THINKING;

	printf("Philosopher %d running.\n", dt->id);
	fflush(stdout);

	while(1){
		switch(state){
			case THINKING:
				printf("Philosopher %d thinking.\n", dt->id);
				fflush(stdout);
				sleep(rand()%10);
				state = HUNGRY;
				break;

			case HUNGRY:
				pthread_mutex_lock(&m1);
				printf("Philosopher %d hungry.\n", dt->id);
				fflush(stdout);
				if(pthread_mutex_trylock(dt->fork_right)==0){
					if(pthread_mutex_trylock(dt->fork_left)==0){
						pthread_mutex_unlock(&m1);						
						state = EATING;
					}
					else{
						pthread_mutex_unlock(dt->fork_right);
					}
				}
				else{
				}
				sleep(rand()%10);
				pthread_mutex_unlock(&m1);
				break;

			case EATING:
				printf("Philosopher %d eating.\n", dt->id);
				fflush(stdout);
				sleep(rand()%10);
				pthread_mutex_unlock(dt->fork_left);	//drop the forks after eating
				pthread_mutex_unlock(dt->fork_right);
				state = THINKING;
				break;
		}


	}
}

int main(int argc, char** argv){
	pthread_mutex_t *fork_list;
	unsigned int n_philo, i;
	struct philo_data* dt;
	pthread_t t;

	if(argc < 2){
		printf("Usage: %s <number of philosophers>\n", argv[0]);
		exit(0);
	}

	srand(time(NULL));

	n_philo = atoi(argv[1]);

	fork_list = (pthread_mutex_t *) malloc(n_philo * sizeof(pthread_mutex_t));

	pthread_mutex_init(&m1, NULL);
	pthread_mutex_init(&fork_list[0], NULL);
	for(i=0; i<n_philo; i++){
		dt = (struct philo_data*)malloc(sizeof(struct philo_data));
		dt->id = i;
		dt->fork_right = &fork_list[i];
		if(i != n_philo - 1){
			pthread_mutex_init(&fork_list[i+1], NULL);
			dt->fork_left = &fork_list[i+1];
		}else{
			dt->fork_left = &fork_list[0];
		}
		pthread_create(&t, NULL, philosopher, dt);
	}


	printf("Press any key to finish.\n");
	getchar();

	/*All data allocated will be free finishing the program.*/

	return 0;
}
