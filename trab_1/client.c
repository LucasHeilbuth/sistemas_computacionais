/* This client connects to the server and 
   sends requistions to the server.
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 


#define BUFFER_SIZE 1024
#define PORT 5000

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char sendBuff[1025];
    char recvBuff[1025];
    struct sockaddr_in serv_addr;
    
    

    if (argc < 3){
        printf("Usage: %s <ip address> <list of parameters>\n", argv[0]);
        exit(0);
    }

    
    /*Cria o Socket */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    } 

    /* Configure server adress*/
    memset(&serv_addr, 0, sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        perror("inet_pton");
        return 1;
    } 

    /* Conecta ao servidor. */
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
    	perror("connect");
       	return 1;
    } 
    
    // Limpar buffer e atribuir dizer que é cliente
    memset(sendBuff, 0, BUFFER_SIZE);
    strcpy(sendBuff, "client");
    printf("Enviando mensagem: %s\n", sendBuff);

    if (send(sockfd, sendBuff, BUFFER_SIZE, 0) < 0) {
        perror("Error sending worker hello");
        exit(EXIT_FAILURE);
    }

    // Limpar buffer e atribuir valor da operacao
    memset(sendBuff, 0, BUFFER_SIZE);
    strcpy(sendBuff, argv[2]);
    printf("Enviando mensagem: %s\n", sendBuff);

     /* Manda mensagem da operação */
    if (send(sockfd, sendBuff, BUFFER_SIZE + 1, 0) < 0) {
        perror("Erro envio da operação");
        exit(EXIT_FAILURE);
    }

    return 0;
	/* Aguarda o recebimento de dados do servidor. 
	 * Enquanto n for maior que 0. */
    while ( (n = recv(sockfd, recvBuff, sizeof(recvBuff)-1, 0)) > 0)
    {

    // Limpar buffer
    memset(recvBuff, 0, BUFFER_SIZE);

	/* Coloca null no final da string. */
        recvBuff[n] = '\0';
        if(fputs(recvBuff, stdout) == EOF)
        {
            perror("fputs");
        }
    } 

    return 0;
}
