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
#define MAX_CLIENTS 2

int main(int argc, char *argv[]){
    int server_socket, client_socket[MAX_CLIENTS];
    int has_conn = -1;
    int conn_client = -1;
    int conn_worker = -1;
    struct sockaddr_in serv_addr; 
    struct sockaddr_in client_addr; 
    unsigned int addrlen = sizeof(client_addr);
    char recvBuff[1025];
    char sendBuff[1025];

    /* Cria o Socket: SOCK_STREAM = TCP */
    if( (server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
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
    if (bind(server_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
    	perror("bind");
    	return 1;
    }


	/* Inicia a escuta na porta */
    listen(server_socket, 10);

    while(1) {
        for (int i = 0; i < MAX_CLIENTS; i++) {
		/* Aguarda a conexão */
        client_socket[i] = accept(server_socket, (struct sockaddr*)&client_addr, &addrlen);
         /* Recebe resposta de qual é a origem */
        memset(recvBuff, 0, BUFFER_SIZE);

        if (recv(client_socket[i], recvBuff, BUFFER_SIZE, 0) < 0) {
            perror("Error receiving request");
            exit(EXIT_FAILURE);
        }

         /* Printa a origem*/

        printf("Mensagem recebida: %s\n", recvBuff);


        /* Se for cliente envia a mensagem para o worker */
        if (strcmp(recvBuff, "client") == 0) {
            conn_client = i;

            // Limpar o buffer
            memset(recvBuff, 0, BUFFER_SIZE);

            // Recebe a operação do cliente
            if (recv(client_socket[0], recvBuff, BUFFER_SIZE, 0) < 0) {
                perror("Error receiving request");
                exit(EXIT_FAILURE);
            }
            printf("Mensagem recebida: %s\n", recvBuff);

            // Envia operação ao worker
            strcpy(sendBuff, recvBuff);

                while (conn_worker < 0) {
                    client_socket[!conn_client] = accept(server_socket, (struct sockaddr*)&client_addr, &addrlen);
                    // Recebe mensagem worker para enviar
                    if (recv(client_socket[!conn_client], recvBuff, BUFFER_SIZE, 0) < 0) {
                        perror("Error receiving request");
                        exit(EXIT_FAILURE);
                    }
                    if (strcmp(recvBuff, "worker") == 0) {
                        conn_worker = !conn_client;
                    // Mandar mensagem operacao
                        if (send(client_socket[!conn_client], sendBuff, BUFFER_SIZE + 1, 0) < 0) {
                            perror("Erro envio da operação");
                            exit(EXIT_FAILURE);
                        }
                    }
                }

        }

        // Limpar o buffer
        memset(recvBuff, 0, sizeof(recvBuff));


		/* Fecha conexão com o cliente. */
        close(client_socket[i]);
        conn_client = -1;
        conn_worker = -1;

     }
     }
}
