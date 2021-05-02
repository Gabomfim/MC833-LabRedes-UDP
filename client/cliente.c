

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>


#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include "client_requests.h"

/*typedef struct Profile {
    char email[30];
    char name[30];
    char surname[30];
    char address[100];
    char education[100];
    char graduationYear[4];
    Experience *experience[100]; //vetor de experiencias
    Skill *skills[100]; //vetor de skills
} Profile;*/

int main(){

    strcpy(request, "Connect\n");
    strcpy(response, "\0");

    printf("Conectando ao servidor.....\n");
    printf("Aguarde.....\n");
    //creating a TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //socket address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(50000);
    server_address.sin_addr.s_addr = inet_addr("172.30.164.23"); //o IP local do servidor vai aqui

    int status;
    status = connect(serverSocket, (sockaddr *) &server_address, sizeof(server_address));
    if(status == -1){
        perror("Connection failed");
        close(serverSocket);
        return 1;
    }

    printf("Servidor Conectado!\n");

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
        scanf("%c", &op);
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
            cleanBuffer();
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
        cleanBuffer();
    }
    return 0;
}