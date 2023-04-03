#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<semaphore.h>

#define TOTAL_IPS   10000
#define BUFFERSZ    10
#define STRLEN      64
#define NCON        5



static char buffer[BUFFERSZ][STRLEN];
static int in, out;
static volatile int count;
pthread_mutex_t m =PTHREAD_MUTEX_INITIALIZER;

sem_t s_prod, s_cons;

static char format[] = "ping -c 1 -w 1 %s";
static char command[128];


int ping(char *ip)
{
    char str[1024];
    FILE *fpipe;
    
    
    sprintf(command, format, ip);

    /* executa comando redirecionando a saida para o pipe. */
    if ( (fpipe = popen(command, "r"))<0){
        perror("popen");
    }
    
    /* Lê a primeira linha da resposta do comando ping. */
    /* PING 10.32.175.200 (10.32.175.200) 56(84) bytes of data. */
    fgets(str, 1023, fpipe);

    /*Remove quebra de linha do final da string.*/
    str[strlen(str)-1] = '\0';
    printf("%s", str);

    /* Verifica se teve sucesso no ping com a segunda linha, 
        verificando a segunda linha.

        Se houver apenas uma quebra de linha ("\n"), significa 
        que o host não respondeu. Caso contrário, a resposta
        será semelhante a linha abaixo:
        
        64 bytes from 10.32.175.200: icmp_req=1 ttl=64 time=0.023 ms 

    */

    fgets(str, 1023, fpipe);

    if(strcmp(str, "\n"))
        printf("Alive.\n");
    
    else
        printf("Not responding.\n");


    fclose(fpipe);
    
    return 0;
}


void getFromBuf(char *str){
    sem_wait(&s_cons);
    pthread_mutex_lock(&m); 
    strncpy(str, buffer[out], STRLEN);
    out = (out + 1) % BUFFERSZ;
    pthread_mutex_unlock(&m);
    sem_post(&s_prod);
}

void setToBuf(char *str){
	sem_wait(&s_prod);
    pthread_mutex_lock(&m);
    strncpy(buffer[in], str, STRLEN);
    in = (in + 1) % BUFFERSZ;
    pthread_mutex_unlock(&m);
    sem_post(&s_cons);
}

/* Thread Produtora */
void * producer(void * n){
    int num = *(int*)n;
    char str[STRLEN];
    int i = 0;
    
    while(num--){
        sprintf(str, "%d.%d.%d.%d", rand()%126+1, rand()%256, rand()%256, rand()%256);
        setToBuf(str);
    }
    
}

/* Thread Consumidora */
void * consumer(void * d){
    int id = *(int*)d;
    char str[STRLEN];
    int i = 0;
    
    while(1){
        getFromBuf(str);
        ping(str);
        fflush(stdout);
    }
}


int main(int argc, char **argv){
    pthread_t prod, *con;
    int *ids;
    int i, numthr;

    int n_ips = TOTAL_IPS;

    srand(time(0));
    sem_init(&s_prod,0,BUFFERSZ);
    sem_init(&s_cons,0,0);
    in = 0;
    out = 0;
    count = 0;
    memset(buffer, 0, BUFFERSZ*STRLEN);
    
    if (argc < 2){
        printf("Uso: %s <num threads>\n", argv[0]);
        exit(0);
    }
    
    /*Cria array com os números de identificação das threads. */
    numthr = atoi(argv[1]);
    ids = (int *)malloc(numthr * sizeof(int));
    for(i=0; i<numthr; i++){
        ids[i] = i;
    }
    
    
    /* Cria uma thread produtora */
    if(pthread_create(&prod, NULL, producer, &n_ips)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    /* Cria diversas threads consumidoras */
    con = (pthread_t *)malloc(numthr * sizeof(pthread_t));
    for(i=0; i<numthr; i++){
        if(pthread_create(&con[i], NULL, consumer, (void*)&ids[i])) {
            fprintf(stderr, "Error creating thread\n");
            return 2;
        }
    }

    /* Aguarda o término da thread produtora */
    if(pthread_join(prod, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 3;
    }

    /* Aguarda o esvaziamento do buffer. */
    while (count > 0){
        sleep(1);
    }

    return 0;
}

