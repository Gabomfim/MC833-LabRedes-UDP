// Aqui estão definidas as funções que leem informações que os clientes desejam requisitar ao servidor. Todas as funções tratam essas informações para Json com auxilio 
//  do  arquivo JSONfier_client e enviam as requisições com a função sendToServer().
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "JSONfier_client.h"

//Global variables
int serverSocket;
struct sockaddr_in server_address;
unsigned int server_len = sizeof(server_address);
char request[MAX_LINE];


bool createProfile();
bool addExperience();
bool addSkill();
bool list();
bool listProfilesBasedOnSkill();
bool listProfilesBasedOnEducation();
bool listProfilesBasedOnGraduationYear();
bool listAllProfiles();
bool readProfile();
bool removeProfile();
int formatJson(char* json, int len);
void cleanBuffer();
bool sendToServer(char* json);

int formatJson(char* json, int len){
    int n = len;
    json[n] = '\n';
    n++;
    json[n] = '\0';
    n++;
    return n;
}

bool sendToServer(char* json){
    int n = 0, status;
    char* response = malloc(sizeof(char)*5000);
    while(json[n]!='\0'){
        n++;
    }    
    n = formatJson(json, n);
    status = sendto(serverSocket, json, n, 0, (struct sockaddr*)&server_address, server_len);


    if(status < 0){
        printf("Send failed, error code %d\n", status);
        return false;
    }
    //receive message from server;
    status = recvfrom(serverSocket, response, 5000, 0, (struct sockaddr*)&server_address, &server_len);
    if(status <= 0){
        printf("Recv failed, error code %d\n", status);
        return false;
    }
    printf("RESPONSE IS: %s", response);
    bool retorno = parseServerMessage(response, status);
    free(response);
    //treat server response
    return retorno;
}

bool createProfile(){
    Profile profile;
    bool status;

    printf("\n\n------------------------------------------");
    printf("\n        1. Criar perfil\n");
    printf("------------------------------------------\n\n");
    
    printf("-> Insira o email do perfil: ");
    fgets(profile.email, MAX_Name, stdin);
    profile.email[strcspn(profile.email, "\n")] = '\0';

    printf("-> Insira o primeiro nome do perfil: ");
    fgets(profile.name, MAX_Name, stdin);
    profile.name[strcspn(profile.name, "\n")] = '\0';

    printf("-> Insira o último nome do perfil: ");
    fgets(profile.surname, MAX_Name, stdin);
    profile.surname[strcspn(profile.surname, "\n")] = '\0';

    printf("-> Insira o endereço do perfil: ");
    fgets(profile.address, MAX_CARAC, stdin);
    profile.address[strcspn(profile.address, "\n")] = '\0';

    printf("-> Insira o curso do perfil: ");
    fgets(profile.education, MAX_CARAC, stdin);
    profile.education[strcspn(profile.education, "\n")] = '\0';

    printf("-> Insira o ano de conclusão de curso do perfil: ");
    fgets(profile.graduationYear, MAX_Year, stdin);
    profile.graduationYear[strcspn(profile.graduationYear, "\n")] = '\0';

    printf("\n\nEnviando informações ao servidor\n");
    printf("-----------------Aguarde-----------------\n\n");
    
    //CRIA STRINGS JSON PARA AS REQUESTS AO SERVIDOR
    char* json = (char*) createJson(profile);
    status = sendToServer(json);

    return status;
}

bool addExperience(){
    Profile profile;
    char** experience;
    int num_experiences;
    bool status;

    printf("\n\n------------------------------------------");
    printf("\n    2. Adicionar experiência à perfil\n");
    printf("------------------------------------------\n\n");

    printf("-> Insira o email do perfil: ");
    fgets(profile.email, MAX_Name, stdin);
    profile.email[strcspn(profile.email, "\n")] = '\0';
    
    printf("-> Quantas experiências você gostaria de adicionar à %s?\nR: ", profile.email);
    scanf("%d", &num_experiences);
    getchar();
    experience = calloc(num_experiences, sizeof(char*));

    int i;
    for(i = 0; i < num_experiences; i++){
        experience[i] = calloc(MAX_CARAC, sizeof(char));
        printf("-> Insira a %d° experiência profissonal do perfil: ", i+1);
        fgets(experience[i], MAX_CARAC, stdin);
        experience[i][strcspn(experience[i], "\n")] = '\0';    
    }
    
    profile.experience = (const char * const*)experience;

    printf("\n\nEnviando informações ao servidor\n");
    printf("-----------------Aguarde-----------------\n\n");

    //Tratamento dos dados e comunicação com o servidor
    char* json = addExperienceJson(profile, num_experiences);
    status = sendToServer(json);
        //Liberando variável alocada
    for(i = 0; i < num_experiences; i++){
        free(experience[i]);
    }
    free(experience);

    return status;
}

bool addSkill(){
    Profile profile;
    char** skill;
    int num_skills;
    bool status;

    printf("\n\n------------------------------------------");
    printf("\n    3. Adicionar habilidade à perfil\n");
    printf("------------------------------------------\n\n");

    printf("-> Insira o email do perfil: ");
    fgets(profile.email, MAX_Name, stdin);
    profile.email[strcspn(profile.email, "\n")] = '\0';

    printf("-> Quantas habilidades você gostaria de adicionar à %s?\nR: ", profile.email);
    scanf("%d", &num_skills);
    getchar();
    skill = calloc(num_skills, sizeof(char*));

    int i;
    for(i = 0; i < num_skills; i++){
        skill[i] = calloc(MAX_CARAC, sizeof(char));
        printf("-> Insira a %d° habilidade do perfil: ", i+1);
        fgets(skill[i], MAX_CARAC, stdin);
        skill[i][strcspn(skill[i], "\n")] = '\0';
    }
    profile.skills = (const char * const*)skill;

    printf("\n\nEnviando informações ao servidor\n");
    printf("-----------------Aguarde-----------------\n\n");

    //Tratamento dos dados e comunicação com o servidor
    char* json = addSkillJson(profile, num_skills);
    status = sendToServer(json);

    //Liberando variável alocada
    for(i = 0; i < num_skills; i++){
        free(skill[i]);
    }
    free(skill);

    return status;
}

bool readProfile(){
    char email[MAX_Name];
    printf("\n\n------------------------------------------");
    printf("\n    a. Ler perfil a partir de email\n");
    printf("------------------------------------------\n\n");

    printf("-> Insira o email do perfil: ");
    fgets(email, MAX_Name, stdin);
    email[strcspn(email, "\n")] = '\0';

    printf("\n\nEnviando informações ao servidor\n");
    printf("-----------------Aguarde-----------------\n\n");

    //Tratamento dos dados e comunicação com o servidor
    char* json = listJson('u', email);

    return sendToServer(json);;
}

bool listAllProfiles(){
    printf("\n\n------------------------------------------");
    printf("\n    b. Listar todos os perfis existentes\n");
    printf("------------------------------------------\n\n");

    printf("\n\nRecuperando informações do servidor\n");
    printf("-----------------Aguarde-----------------\n\n");

    //Tratamento dos dados e comunicação com o servidor
    char* json = listJson('a', "ALL");

    return sendToServer(json);;
}

bool listProfilesBasedOnSkill(){
    char skill[MAX_CARAC];

    printf("\n\n------------------------------------------");
    printf("\n    c. Listar baseado em habilidade\n");
    printf("------------------------------------------\n\n");

    printf("-> Insira a skill que procura: ");
    fgets(skill, MAX_CARAC, stdin);
    skill[strcspn(skill, "\n")] = '\0';

    printf("\n\nEnviando informações ao servidor\n");
    printf("-----------------Aguarde-----------------\n\n");

    //Tratamento dos dados e comunicação com o servidor
    char* json = listJson('s', skill);

    return sendToServer(json);;
}

bool listProfilesBasedOnEducation(){
    char education[MAX_CARAC];

    printf("\n\n------------------------------------------");
    printf("\n    d. Listar baseado em curso\n");
    printf("------------------------------------------\n\n");

    printf("-> Insira o curso que procura:");
    fgets(education, MAX_CARAC, stdin);
    education[strcspn(education, "\n")] = '\0';

    printf("\n\nEnviando informações ao servidor\n");
    printf("-----------------Aguarde-----------------\n\n");

    //Tratamento dos dados e comunicação com o servidor
    char* json = listJson('e', education);

    return sendToServer(json);;
}

bool listProfilesBasedOnGraduationYear(){
    char graduationYear[MAX_Year];

    printf("\n\n------------------------------------------");
    printf("\n e. Listar baseado em ano de conclusão\n");
    printf("------------------------------------------\n\n");

    printf("-> Insira o ano de conclusão que procura(4 números): ");
    fgets(graduationYear, MAX_Year, stdin);
    graduationYear[strcspn(graduationYear, "\n")] = '\0';

    printf("\n\nEnviando informações ao servidor\n");
    printf("-----------------Aguarde-----------------\n\n");

     //Tratamento dos dados e comunicação com o servidor
    char* json = listJson('y', graduationYear);

    return sendToServer(json);;
}

bool list(){

    char letter = '0';
    int flag = 1;
    bool retorno = true;
    while(flag){
        printf("\n\n------------------------------------------\n");
        printf("         Listagem de usuários!!!\n");
        printf("------------------------------------------\n\n");
        printf("Qual listagem de usuários você gostaria de realizar?\n"
                "   a. Ler perfil a partir de email\n"
                "   b. Listar todos os perfis existentes\n"
                "   c. Listar todos os perfis com uma certa habilidade\n"
                "   d. Listar todos os perfis de um determinado curso\n"
                "   e. Listar todos os perfis que se formam em determinado ano\n"
                "   f. Cancelar e retornar ao menu principal\n"
                "Digite o número da opção que deseja realizar: "
        );

        input:
        scanf("%c", &letter);
        if(letter=='\n'){
            goto input;
        }
        getchar();

        switch (letter)
        {
        case 'a'://ler perfil a partir de email
            retorno = readProfile();
            flag = 0;
            break;

        case 'b'://ler todos os perfis
            retorno = listAllProfiles();
            flag = 0;
            break;
        
        case 'c'://ler usuários com determinada habilidade
            retorno = listProfilesBasedOnSkill();
            flag = 0;
            break;

        case 'd'://Ler usuários de determinado curso
            retorno = listProfilesBasedOnEducation();
            flag = 0;
            break;

        case 'e'://ler usuários que se formam em determinado ano
            retorno = listProfilesBasedOnGraduationYear();
            flag = 0;
            break;

        case 'f'://retornar a menu principal
            printf("\n\n----Retornando a menu principal...----\n\n");
            flag = 0;
            break;    

        default://colocar algum caracter invalido
            printf("\n\n------------------------------------------\n");
            printf("--------------Opção-Invalida--------------\n");
            printf("------------------------------------------\n\n");
            break;
        }
        
    }
    return retorno;
}

bool removeProfile(){
    Profile profile;
    printf("\n\n------------------------------------------");
    printf("\n        5. Remover perfil\n");
    printf("------------------------------------------\n\n");

    printf("-> Insira o email do perfil que deseja remover: ");
    fgets(profile.email, MAX_Name, stdin);
    profile.email[strcspn(profile.email, "\n")] = '\0';

    printf("\n\nEnviando informações ao servidor\n");
    printf("-----------------Aguarde-----------------\n\n");

     //Tratamento dos dados e comunicação com o servidor
    char* json = removeJson(profile);

    return sendToServer(json);;
}
