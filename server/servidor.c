/*
    Este arquivo "liga" o servidor, setando a porta e o endereço ip. Após isso, conecta à um cliente e, à medida que mensagens são recebidas, direciona
    essas mensagens à função que realiza o parse do json.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "jsonParser.h"

#define MAX_SIZE 1024

void emergencyExit();

int main(int argc, char *argv[ ]){
  
    if(argc != 2){
        printf("argc: %d\n", argc);
        printf("Erro de execução, olhe o README para mais informações!!\n");
        exit(1);
    }

    server_path = malloc((sizeof(argv[1])*sizeof(char))+1);
    strcpy(server_path, argv[1]);
    
    ssize_t n;

    int serverSocket;
    char request[MAX_SIZE];

    //create the server socket
    if ( (serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    //define server address
    sockaddr_in server_address, client_address;


    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(50000);
    server_address.sin_addr.s_addr = INADDR_ANY;
    
    unsigned int len;
    len = sizeof(client_address); //verificar se datagramas tem tamanho fixo


    //bind the socket to IP and port
     if ( bind(serverSocket, (sockaddr *) &server_address, sizeof(server_address)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while(1){
        listen(serverSocket, 10); //can have 5 connection waiting at max
        again:
        while ( (n = recvfrom(serverSocket, request, MAX_SIZE, 0, ( struct sockaddr *) &client_address, &len)) > 0){
            printf("The client has requested: %s", request);
            printf("buffer size: %ld\n", n);

            parse(request, serverSocket, client_address);

            if (n < 0 && errno == 4)
                goto again;
            else if (n < 0)
                printf("str_echo: recv error");
        }
    } 
    return 0;
}
