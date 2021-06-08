/*
    Arquivo inicial do cliente, que realiza a conexão ao servidor e apresenta um menu de operações ao usuário. Direciona as requisições às funções definidas e, client_requests.h
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include "client_requests.h"

void emergencyExit();

int main(int argc, char *argv[ ]){
    
    if (argc != 1 && argc != 2){
        printf("Erro de execução, olhe o README para mais informações!!\n");
        exit(1);
    }

    //O que acontece quando eu aperto ^C
    signal(SIGINT, emergencyExit);

    char request[] = "Connect\n\0";
    char response[50];

    printf("Conectando ao servidor.....\n");
    printf("Aguarde.....\n");
    //creating a TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //define server address
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(50000);
    if(argc == 1){
        server_address.sin_addr.s_addr = INADDR_ANY;
    }else if (argc == 2){
        server_address.sin_addr.s_addr = inet_addr(argv[1]);
    }
    unsigned int len;
    len = sizeof(server_address); //verificar se datagramas tem tamanho fixo

    //socket address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(50000);
    server_address.sin_addr.s_addr = inet_addr(argv[1]); //o IP local do servidor vai aqui

    int status;
    status = connect(serverSocket, (sockaddr *) &server_address, len);
    if(status == -1){
        perror("Connection failed");
        close(serverSocket);
        return 1;
    }

    status = sendto(serverSocket, request, sizeof(request), 0, (struct sockaddr*)&server_address, len);
    status = recvfrom(serverSocket, response, sizeof(response), 0, (struct sockaddr*)&server_address, &len);
    printf("\nServidor Conectado!\n");

    printf("                                                                   \n"            
           "                                                                   \n"
           "                          ,(/& &//,    (/.                         \n"
           "                   @@@@@@@@@@@ @@@@@@ @@@@,                        \n"
           "               #@@@@@((@@@@@@@ @@@@@@@/#/@@@@*                     \n"
           "             @@@@@@@@@#/@@@@@@ @@@@@@,&@@@@@@@@/                   \n"
           "           @@@#,@@@@@@@&*@@@@@ @@@@@.&@@@@@@@ &@@@                 \n"
           "         .@@@@@@@./@@@@@&,@@@@ @@@@ @@@@@@#*@@@@@@@                \n"
           "         @@@@@@@@@@&.@@@@@.@@@ @@@ @@@@@ @@@@@@@@@@@               \n"
           "        &@@*./@@@@@@@@*(@@@.&@ @@ @@@*(@@@@@@@@#./@@@              \n"
           "        @@@@@@@@@( (@@@@/           &@@@@/.#@@@@@@@@@@             \n"
           "        *@@@@@@@@@@@@@@              ,@@@@@@@@@@@@@@@( *.          \n"
           "         ,,,,,,,,,,,,,,               ,,,,,,,,,,,,,,./@@@@         \n"
           "          &@@@@@@@@@@@#              (@@@@@@@@@@@@@@/              \n"
           "           ,@@@@(.(@@@@@/           #@@@@@@@@@@@@&                 \n"
           "             .@@@@@@@@ @@@@@@@@@@.@@@&.@@@@@@@@.                   \n"
           "                ,@@,#@@@@@@@@@@@@@.&&@@@,(@&                       \n"
           "                     /@@@@@@@@@@@ @@@@.*                           \n"
           "                                  ,@@(                             \n"
           "                                                                   \n"
           "   (/    ,#  /#    #*  @@ .@@@@/(     */,    (@     ,@,  ,@@@@//   \n"
           "   &/    /@  @,@@. &#  @@  @.        @@ @@   @@@/  &@@/  (@____@/  \n"
           "   /@@@@@@@  @.  *@@(  @@  @@@@@@@  @@   @@  /# &@@* @(  (@        \n"
           "                                                                \n\n");
    
    char op = '0';
    bool flag = true;
    bool validation;

    //keep server communication
    while(flag){
        validation = true;
        printf("\n\n------------------------------------------\n");
        printf("         Requisições de cliente!!!\n");
        printf("------------------------------------------\n\n");
        printf("Você gostaria de realizar qual operação?\n"
                "   1. Adicionar novo usuário\n"
                "   2. Adicionar experiência profissional à usuário existente\n"
                "   3. Adicionar habilidade à usuário existente\n"
                "   4. Listar usuário\n"
                "   5. Remover usuário existente\n"
                "   6. Desconectar do servidor\n"
                "Digite o número da opção que deseja realizar: "
        );

        input:
        scanf("%c", &op);
        if(op=='\n'){
            goto input;
        }
        getchar();
        
        switch (op)
        {
        case '1'://criar novo usuário
            validation = createProfile();
            break;

        case '2'://criar novo usuário
            validation = addExperience();
            break;
        
        case '3'://adiciona skill a usuário
            validation = addSkill();
            break;

        case '4'://traz menu para escolha de qual listagem o usuário quer realizar
            validation = list();
            break;

        case '5'://remover perfil de usuário baseado no e-mail
            validation = removeProfile();
            break;

        case '6'://desligar aplicação
            printf("\n\n------------------------------------------\n");
            printf("    Fechando conexão com o servidor....\n");
            printf("------------------------------------------\n\n");
            close(serverSocket);
            printf("-----------Tchau! Volte sempre!-----------\n\n");
            flag = false;
            break;    

        default://colocar algum caracter invalido
            printf("\n\n------------------------------------------\n");
            printf("--------------Opção-Invalida--------------\n");
            printf("------------------------------------------\n\n");
            validation = false;
            break;
        }
        

        if(flag && validation){
            printf("\n--------------Sucesso!!--------------\n");
        }else if(flag && !validation){
            printf("\n--------------Erro!!--------------\n");
        }
    }

    return 0;
}

//O que acontece quando eu aperto ^C
void emergencyExit(){
    printf("\nDesconectando do servidor...\n");
    close(serverSocket);
    exit(0);
}
