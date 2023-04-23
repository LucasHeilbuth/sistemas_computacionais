#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    struct sockaddr_in client_addr; 
    unsigned int addrlen = sizeof(client_addr);
    char recvBuff[1025];
    char sendBuff[1025];

    /* Cria o Socket: SOCK_STREAM = TCP */
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    	perror("socket");
    	return 1;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(sendBuff, 0, BUFFER_SIZE);


	/* Configura servidor para receber conexoes de qualquer endereço:
	 * INADDR_ANY e ouvir na porta 5000 */ 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

	/* Associa o socket a estrutura sockaddr_in */
    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
    	perror("bind");
    	return 1;
    }


	/* Inicia a escuta na porta */
    listen(listenfd, 10); 

    while(1) {

		/* Aguarda a conexão */	
        connfd = accept(listenfd, (struct sockaddr*)&client_addr, &addrlen);

         /* Recebe resposta de qual é a origem */
        memset(recvBuff, 0, BUFFER_SIZE);
        if (recv(connfd, recvBuff, BUFFER_SIZE, 0) < 0) {
            perror("Error receiving request");
            exit(EXIT_FAILURE);
        }

         /* Printa a origem*/

        printf("Mensagem recebida: %s\n", recvBuff);


        /* Se for cliente envia a mensagem para o worker */
        if (strcmp(recvBuff, "client") == 0) {
            // Limpar o buffer
            memset(recvBuff, 0, BUFFER_SIZE);
            // Recebe a operação do cliente
            if (recv(connfd, recvBuff, BUFFER_SIZE, 0) < 0) {
                perror("Error receiving request");
                exit(EXIT_FAILURE);
            }
            printf("Mensagem recebida: %s\n", recvBuff);
            /* Envia operação ao worker. */
           // strcpy(sendBuff, recvBuff);
         // send(connfd, sendBuff, strlen(sendBuff)+1, 0);*/
        }

        /* Se for worker printa a resposta na tela */
        else if (strcmp(recvBuff, "worker") == 0) {
            printf("Resposta da operação: %s", recvBuff);
        }

        // Limpar o buffer
        memset(recvBuff, 0, sizeof(recvBuff));


		/* Fecha conexão com o cliente. */
        close(connfd);

     }
}
