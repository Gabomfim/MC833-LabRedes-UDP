/*
    Neste arquivo temos todo o tratamento de json pelo lado do servidor. A primeira função trata o Json e o direciona à uma das funções específicas para realizar operações 
    no banco de dados.
*/

#include "../lib/cJSON.h"
#include "dao.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool parse(char* JSONstring, int clientSocket, sockaddr_in dest);
bool parseCreate(const cJSON *JSONobject, int clientSocket, sockaddr_in dest);
bool parseAddSkill(const cJSON *JSONobject, int clientSocket, sockaddr_in dest);
bool parseaddExperience(const cJSON *JSONobject, int clientSocket, sockaddr_in dest);
bool parseRemove(const cJSON *JSONobject, int clientSocket, sockaddr_in dest);
bool parseListAll(int clientSocket, sockaddr_in dest);
bool parseListUser(const cJSON *JSONobject, int clientSocket, sockaddr_in dest);
bool parseListEducation(const cJSON *JSONobject, int clientSocket, sockaddr_in dest);
bool parseListSkill(const cJSON *JSONobject, int clientSocket, sockaddr_in dest);
bool parseListYear(const cJSON *JSONobject, int clientSocket, sockaddr_in dest);


//Transforma as strings JSON em structs profile à serem passados para o banco de dados.
bool parse(char* JSONstring, int clientSocket, sockaddr_in dest){

    const cJSON *operation = NULL;
    const cJSON *JSONobject = NULL;
    int status = false;
    cJSON *JSON = cJSON_Parse(JSONstring);

    //Criando um objeto JSON a partir da string recebida
    if(strcmp(JSONstring, "Connect\n")==0){
        printf("Cliente Conectado\n");
        char* response = formatResponse(true, NULL, 0, 0);
        sendResponse(response, clientSocket, dest);
        status = true;
        goto end;
    }else{
        if (JSON == NULL){
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL){
                fprintf(stderr, "[SERVER] Error before: %s\n", error_ptr);
            }
            status = false;
            goto end;
        }

        //obtendo a operação à ser executada
        operation = cJSON_GetObjectItemCaseSensitive(JSON, "operation");
        if (cJSON_IsString(operation) && (operation->valuestring != NULL)){
            printf("Checking operation: \"%s\"\n", operation->valuestring);
        }

        //CREATE
        if(strcmp(operation->valuestring, "CREATE")==0){

            //obtendo o perfil a ser criado
            JSONobject = cJSON_GetObjectItemCaseSensitive(JSON, "object");

            if(!parseCreate(JSONobject, clientSocket, dest)){
                printf("The server couldn't create the profile\n");
                status = false;
                goto end;
            }else{
                printf("Profile created\n");
                status = true;
                goto end;
            }

        }else if(strcmp(operation->valuestring, "ADD_SKILL")==0){

            //obtendo o perfil a ser criado
            JSONobject = cJSON_GetObjectItemCaseSensitive(JSON, "object");

            if(!parseAddSkill(JSONobject, clientSocket, dest)){
                printf("The server couldn't add the skills\n");
                status = false;
                goto end;
            }else{
                printf("Skills added to user\n");
                status = true;
                goto end;
            }

        }else if(strcmp(operation->valuestring, "ADD_EXP")==0){

            //obtendo o perfil a ser criado
            JSONobject = cJSON_GetObjectItemCaseSensitive(JSON, "object");

            if(!parseaddExperience(JSONobject, clientSocket, dest)){
                printf("The server couldn't add the experiences\n");
                status = false;
                goto end;
            }else{
                printf("Experiences added to user\n");
                status = true;
                goto end;
            }

        }else if(strcmp(operation->valuestring, "REMOVE")==0){

            //obtendo o perfil a ser criado
            JSONobject = cJSON_GetObjectItemCaseSensitive(JSON, "object");

            if(!parseRemove(JSONobject, clientSocket, dest)){
                printf("The server couldn't remove the user\n");
                status = false;
                goto end;
            }else{
                printf("User removed\n");
                status = true;
                goto end;
            }

        }else if(strcmp(operation->valuestring, "ListAll")==0){

            if(!parseListAll(clientSocket, dest)){
                printf("The server couldn't list the users\n");
                status = false;
                goto end;
            }else{
                printf("Users listed\n");
                status = true;
                goto end;
            }

        }else if(strcmp(operation->valuestring, "ListUser")==0){

            //obtendo o perfil a ser criado
            JSONobject = cJSON_GetObjectItemCaseSensitive(JSON, "object");

            if(!parseListUser(JSONobject, clientSocket, dest)){
                printf("The server couldn't list the users\n");
                status = false;
                goto end;
            }else{
                printf("Users listed\n");
                status = true;
                goto end;
            }

        }else if(strcmp(operation->valuestring, "ListEducation")==0){

            //obtendo o perfil a ser criado
            JSONobject = cJSON_GetObjectItemCaseSensitive(JSON, "object");

            if(!parseListEducation(JSONobject, clientSocket, dest)){
                printf("The server couldn't list the users\n");
                status = false;
                goto end;
            }else{
                printf("Users listed\n");
                status = true;
                goto end;
            }

        }else if(strcmp(operation->valuestring, "ListSkill")==0){

            //obtendo o perfil a ser criado
            JSONobject = cJSON_GetObjectItemCaseSensitive(JSON, "object");

            if(!parseListSkill(JSONobject, clientSocket, dest)){
                printf("The server couldn't list the users\n");
                status = false;
                goto end;
            }else{
                printf("Users listed\n");
                status = true;
                goto end;
            }

        }else if(strcmp(operation->valuestring, "ListYear")==0){

            //obtendo o perfil a ser criado
            JSONobject = cJSON_GetObjectItemCaseSensitive(JSON, "object");

            if(!parseListYear(JSONobject, clientSocket, dest)){
                printf("The server couldn't list the users\n");
                status = false;
                goto end;
            }else{
                printf("Users listed\n");
                status = true;
                goto end;
            }

        }else{

            printf("The server couldn't parse the operation\n");
            char* response = formatResponse(false, NULL, 0, 0);
            sendResponse(response, clientSocket, dest);
            status = false;
            goto end;

        } 
    }
    end:
        cJSON_Delete(JSON);
        return status;
}

/*
----------------------------
--------   CREATE   --------
----------------------------
*/

bool parseCreate(const cJSON *JSONobject, int clientSocket, sockaddr_in dest){

    //CRIA O STRUCT PROFILE COM OS DADOS DO PERFIL
    Profile profile;
    bool status = false;

    printf("PARSING...\n");
    printf("%s\n", cJSON_GetObjectItemCaseSensitive(JSONobject, "email")->valuestring);
    strcpy(profile.email, cJSON_GetObjectItemCaseSensitive(JSONobject, "email")->valuestring);
    printf("-> Email: %s\n", profile.email);

    strcpy(profile.name, cJSON_GetObjectItemCaseSensitive(JSONobject, "name")->valuestring);
    printf("-> Name: %s\n", profile.name);

    strcpy(profile.surname, cJSON_GetObjectItemCaseSensitive(JSONobject, "surname")->valuestring);
    printf("-> Surname: %s\n", profile.surname);

    strcpy(profile.address, cJSON_GetObjectItemCaseSensitive(JSONobject, "address")->valuestring);
    printf("-> Address: %s\n", profile.address);

    strcpy(profile.education, cJSON_GetObjectItemCaseSensitive(JSONobject, "education")->valuestring);
    printf("-> Education: %s\n", profile.education);

    strcpy(profile.graduationYear, cJSON_GetObjectItemCaseSensitive(JSONobject, "graduationYear")->valuestring);
    printf("-> Graduation Year: %s\n", profile.graduationYear);

    //CRIA O PERFIL NO BANCO DE DADOS
    if(createProfile(profile, clientSocket, dest)){
        printf("Data inserted in the database\n");
        status = true;
    }else{
        printf("Couldn't insert the data in the database\n");
        status = false;
    }

    return status;
}

/*
* !!!!!!!!!!!!!!AJUSTAR addSkill e addExperience!!!!!!!!!!!!!!!!!!!
*/
bool parseAddSkill(const cJSON *JSONobject, int clientSocket, sockaddr_in dest){

    char email[31];
    cJSON* SkillsJson = cJSON_GetObjectItemCaseSensitive(JSONobject, "skills");
    char* skill;
    int size = cJSON_GetArraySize(SkillsJson), aux;
    bool status = true;

    strcpy(email, cJSON_GetObjectItemCaseSensitive(JSONobject, "email")->valuestring);
    printf("-> Email: %s\n", email);

    for(aux = 0; aux < size; aux++){
        skill = cJSON_GetArrayItem(SkillsJson, aux)->valuestring;
        printf("-> Experience %d: %s\n", aux, skill);
        if(!addSkill(email, skill, dest)){
            status = false;
        }
    }
    char* response;
    if(status == true){
        response = formatResponse(true, NULL, 0, 0);
    }else{
        response = formatResponse(false, NULL, 0, 0);
    }
    return sendResponse(response, clientSocket, dest);
}

bool parseaddExperience(const cJSON *JSONobject, int clientSocket, sockaddr_in dest){

    char email[31];
    cJSON* ExpJson = cJSON_GetObjectItemCaseSensitive(JSONobject, "experiences");
    char* exp;
    int size = cJSON_GetArraySize(ExpJson), aux;
    bool status = true;

    strcpy(email, cJSON_GetObjectItemCaseSensitive(JSONobject, "email")->valuestring);
    printf("-> Email: %s\n", email);

    for(aux = 0; aux < size; aux++){
        exp = cJSON_GetArrayItem(ExpJson, aux)->valuestring;
        printf("-> Experience %d: %s\n", aux, exp);
        if(!addExperience(email, exp, dest)){
            status = false;
        }
    }

    char* response;
    if(status == true){
        response = formatResponse(true, NULL, 0, 0);
    }else{
        response = formatResponse(false, NULL, 0, 0);
    }
    return sendResponse(response, clientSocket, dest);
}

bool parseRemove(const cJSON *JSONobject, int clientSocket, sockaddr_in dest){
    char email[31];

    strcpy(email, cJSON_GetObjectItemCaseSensitive(JSONobject, "email")->valuestring);
    printf("-> Email: %s\n", email);

    return removeProfile(email, clientSocket, dest);
}

bool parseListAll(int clientSocket, sockaddr_in dest){
    return listAllProfiles(clientSocket, dest);
}

bool parseListUser(const cJSON *JSONobject, int clientSocket, sockaddr_in dest){

    char email[31];

    strcpy(email, cJSON_GetObjectItemCaseSensitive(JSONobject, "email")->valuestring);
    printf("-> Email: %s\n", email);

    return readProfile(email, clientSocket, dest);
}

bool parseListEducation(const cJSON *JSONobject, int clientSocket, sockaddr_in dest){

    char education[101];

    strcpy(education, cJSON_GetObjectItemCaseSensitive(JSONobject, "education")->valuestring);
    printf("-> Education: %s\n", education);

    return listProfilesBasedOnEducation(education, clientSocket, dest);
}

bool parseListSkill(const cJSON *JSONobject, int clientSocket, sockaddr_in dest){

    char skill[101];

    strcpy(skill, cJSON_GetObjectItemCaseSensitive(JSONobject, "skill")->valuestring);
    printf("-> Skill: %s\n", skill);

    return listProfilesBasedOnSkill(skill, clientSocket, dest);
}

bool parseListYear(const cJSON *JSONobject, int clientSocket, sockaddr_in dest){

    char graduationYear[5];

    strcpy(graduationYear, cJSON_GetObjectItemCaseSensitive(JSONobject, "graduationYear")->valuestring);
    printf("-> Graduation Year: %s\n", graduationYear);

    return listProfilesBasedOnGraduationYear(graduationYear, clientSocket, dest);
}
