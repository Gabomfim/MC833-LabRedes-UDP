/*
    Neste arquivo definimos todas as funções de acesso ao banco de dados para realizar as opereções requisitadas pelos usuários. Após a operação no banco, as funções chamam
    a função que manda um json de resposta ao cliente com o que foi processado pelo banco. O arquivo do banco é o app.db e seu caminho deve ser passado na execução do servidor.
*/

#include<sqlite3.h>
#include<stdio.h>
#include "../lib/cJSON.h"
#include "../lib/profileTypes.h"

#define QUERY_MAX_SIZE 512
#define MAX_COLUMN 10
#define MAX_ROW 1000

typedef struct JSONprofile {
    cJSON *email;
    cJSON *name;
    cJSON *surname;
    cJSON *address;
    cJSON *education;
    cJSON *graduationYear;
    cJSON *experience; //vetor de experiencias
    cJSON *skills; //vetor de skills
} JSONprofile;

typedef struct JSONquery {
    cJSON *wrapper;
    cJSON *operation;
    cJSON *object;
} JSONquery;

char* server_path;

bool createProfile (Profile profile, int clientSocket, sockaddr_in dest);
bool addExperience(char email[30], char professionalExperience[100], sockaddr_in dest);
bool addSkill(char email[30], char skill[100], sockaddr_in dest);
bool listProfilesBasedOnSkill(char skill[100], int clientSocket, sockaddr_in dest);
bool listProfilesBasedOnEducation(char education[100], int clientSocket, sockaddr_in dest);
bool listProfilesBasedOnGraduationYear(char graduationYear[4], int clientSocket, sockaddr_in dest);
bool listAllProfiles(int clientSocket, sockaddr_in dest);
bool readProfile(char *email, int clientSocket, sockaddr_in dest);
bool removeProfile(char *email, int clientSocket, sockaddr_in dest);
int callback(void *, int, char **, char **);
char* formatResponse(bool status, char** data, int lineCount, int collumnCount);
cJSON* formatProfiles(char** data, int lineCout, int collumnCount);
bool sendResponse(char* response, int clientSocket, sockaddr_in dest);
int formatJson(char* json, int len);

/*
----------------------------
--------   CREATE   --------
----------------------------
*/

int formatJson(char* json, int len){
    int n = len;
    json[n] = '\n';
    n++;
    json[n] = '\0';
    n++;
    return n;
}

bool sendResponse(char* response, int clientSocket, sockaddr_in dest){
    int n = 0, status;
    while(response[n]!='\0'){
        n++;
    }    
    n = formatJson(response, n);
    status = sendto(clientSocket, response, n, 0, (struct sockaddr*)&dest, sizeof(dest));


    if(status < 0){
        printf("Send failed, error code %d\n", status);
        return false;
    }
    return true;
}

bool createProfile(Profile profile, int clientSocket, sockaddr_in dest){
   
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(server_path, &db); //caminho relativo à onde você executa o programa no terminal
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return false;
    }
    
    //Query
    char sql[QUERY_MAX_SIZE];
    strcpy(sql, "INSERT INTO Profiles VALUES('");
    strcat(sql, profile.email);
    strcat(sql, "', '");
    strcat(sql, profile.name);
    strcat(sql, "', '");
    strcat(sql, profile.surname);
    strcat(sql, "', '");
    strcat(sql, profile.address);
    strcat(sql, "', '");
    strcat(sql, profile.education);
    strcat(sql, "', '");
    strcat(sql, profile.graduationYear);
    strcat(sql, "');");

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        return false;
    }
    char* response = formatResponse(true, NULL, 0, 0);

    sqlite3_close(db);

    return sendResponse(response, clientSocket, dest);
}

/*
------------------------------------
--------   ADD EXPERIENCE   --------
------------------------------------
*/

bool addExperience(char email[30], char experience[100], sockaddr_in dest){
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(server_path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return false;
    }

    //Query
    char sql[QUERY_MAX_SIZE];
    strcpy(sql, "INSERT INTO Experiences VALUES(NULL, '");
    strcat(sql, email);
    strcat(sql, "', '");
    strcat(sql, experience);
    strcat(sql, "');");

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);

    return true;
}

/*
-------------------------------
--------   ADD SKILL   --------
-------------------------------
*/

bool addSkill(char email[30], char skill[100], sockaddr_in dest){

    sqlite3 *db;
    char *err_msg = 0;
    char *sql;
    
    int rc = sqlite3_open(server_path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return false;
    }

    //Query 1

    sql = (char *) malloc(QUERY_MAX_SIZE*sizeof(char));
    strcpy(sql, "INSERT INTO Skills VALUES('");
    strcat(sql, skill);
    strcat(sql, "');");

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        free(sql);
        return false;
    }
    free(sql);

    //Query 2
    sql = (char *) malloc(QUERY_MAX_SIZE*sizeof(char));;
    strcpy(sql, "INSERT INTO Profiles_Skills VALUES('");
    strcat(sql, email);
    strcat(sql, "', '");
    strcat(sql, skill);
    strcat(sql, "');");


    strcat(sql, "");

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        free(sql);
        return false;
    }
    free(sql);

    sqlite3_close(db);

    return true;
}

/*
---------------------------------------------
--------   LIST BASED ON EDUCATION   --------
---------------------------------------------
*/

bool listProfilesBasedOnEducation(char education[100], int clientSocket, sockaddr_in dest){
    bool status = true;
    sqlite3 *db;
    char **sql_return;
    char *err_msg = 0;
    int n, linecount, collumncount, i;
    
    int rc = sqlite3_open(server_path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return false;
    }
    
    char sql[QUERY_MAX_SIZE];
    strcpy(sql, "SELECT * FROM Profiles WHERE Education == '");
    strcat(sql, education);
    strcat(sql, "';");
    sqlite3_stmt *stmt;

    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    rc = sqlite3_prepare_v2(db, sql, QUERY_MAX_SIZE, &stmt, (const char**)&err_msg);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        status = false;
        goto end;
    }

    n = sqlite3_column_count(stmt);
    if(n > MAX_COLUMN){
        printf("Max collumn count reached\n");
        status = false;
        goto end;
    }

    linecount = 0;
    sql_return = malloc(sizeof(char*)*MAX_LINE*MAX_COLUMN);
    do{
        rc = sqlite3_step(stmt);
        
        if (rc == SQLITE_ROW) {
            for(collumncount = 0; collumncount < n; collumncount++){
                sql_return[n*linecount+collumncount] = malloc(sizeof(char)*MAX_CARAC);
                strcpy(sql_return[n*linecount+collumncount], (const char*)sqlite3_column_text(stmt, collumncount));
            }
        }

        linecount++;
        if(linecount == MAX_ROW){
            printf("Max line count reached\n");
            break;
        }
    }while(rc != SQLITE_DONE);
    
    char* stringRetorno = formatResponse(status, sql_return, linecount-1, n);


    for(i = 0; i < linecount - 1 ; i++){
        for(collumncount = 0; collumncount < n; collumncount++){
            free(sql_return[n*linecount+collumncount]);
        }
    }
    free(sql_return);
    status = sendResponse(stringRetorno, clientSocket, dest);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    end:
        return status;
}

/*
-----------------------------------------
--------   LIST BASED ON SKILL   --------
-----------------------------------------
*/

bool listProfilesBasedOnSkill(char skill[300], int clientSocket, sockaddr_in dest){
    bool status = true;
    sqlite3 *db;
    char **sql_return;
    char *err_msg = 0;
    int n, linecount, collumncount, i;
    
    int rc = sqlite3_open(server_path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return false;
    }
    
    char sql[QUERY_MAX_SIZE];
    strcpy(sql, "SELECT * FROM Profiles_Skills, Profiles WHERE Profiles_Skills.Profile_Email = Profiles.Email AND Skill_Description == '");
    strcat(sql, skill);
    strcat(sql, "';");
    sqlite3_stmt *stmt;

    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    rc = sqlite3_prepare_v2(db, sql, QUERY_MAX_SIZE, &stmt, (const char**)&err_msg);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        status = false;
        goto end;
    }

    n = sqlite3_column_count(stmt);
    if(n > MAX_COLUMN){
        printf("Max collumn count reached\n");
        status = false;
        goto end;
    }

    linecount = 0;
    sql_return = malloc(sizeof(char*)*MAX_LINE*MAX_COLUMN);
    do{
        rc = sqlite3_step(stmt);
        
        if (rc == SQLITE_ROW) {
            for(collumncount = 0; collumncount < n; collumncount++){
                sql_return[n*linecount+collumncount] = malloc(sizeof(char)*MAX_CARAC);
                strcpy(sql_return[n*linecount+collumncount], (const char*)sqlite3_column_text(stmt, collumncount));
            }
        }

        linecount++;
        if(linecount == MAX_ROW){
            printf("Max line count reached\n");
            break;
        }
    }while(rc != SQLITE_DONE);
    
    char* stringRetorno = formatResponse(status, sql_return, linecount-1, n);


    for(i = 0; i < linecount - 1 ; i++){
        for(collumncount = 0; collumncount < n; collumncount++){
            free(sql_return[n*linecount+collumncount]);
        }
    }
    free(sql_return);
    status = sendResponse(stringRetorno, clientSocket, dest);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    end:
        return status;
}

/*
---------------------------------------------------
--------   LIST BASED ON GRADUATION YEAR   --------
---------------------------------------------------
*/

bool listProfilesBasedOnGraduationYear(char graduationYear[4], int clientSocket, sockaddr_in dest){
    bool status = true;
    sqlite3 *db;
    char **sql_return;
    char *err_msg = 0;
    int n, linecount, collumncount, i;
    
    int rc = sqlite3_open(server_path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return false;
    }
    
    char sql[QUERY_MAX_SIZE];
    strcpy(sql, "SELECT * FROM Profiles WHERE Graduation_Year == ");
    strcat(sql, graduationYear);
    strcat(sql, ";");
    sqlite3_stmt *stmt;

    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    rc = sqlite3_prepare_v2(db, sql, QUERY_MAX_SIZE, &stmt, (const char**)&err_msg);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        status = false;
        goto end;
    }

    n = sqlite3_column_count(stmt);
    if(n > MAX_COLUMN){
        printf("Max collumn count reached\n");
        status = false;
        goto end;
    }

    linecount = 0;
    sql_return = malloc(sizeof(char*)*MAX_LINE*MAX_COLUMN);
    do{
        rc = sqlite3_step(stmt);
        
        if (rc == SQLITE_ROW) {
            for(collumncount = 0; collumncount < n; collumncount++){
                sql_return[n*linecount+collumncount] = malloc(sizeof(char)*MAX_CARAC);
                strcpy(sql_return[n*linecount+collumncount], (const char*)sqlite3_column_text(stmt, collumncount));
            }
        }

        linecount++;
        if(linecount == MAX_ROW){
            printf("Max line count reached\n");
            break;
        }
    }while(rc != SQLITE_DONE);
    
    char* stringRetorno = formatResponse(status, sql_return, linecount-1, n);


    for(i = 0; i < linecount - 1 ; i++){
        for(collumncount = 0; collumncount < n; collumncount++){
            free(sql_return[n*linecount+collumncount]);
        }
    }
    free(sql_return);
    status = sendResponse(stringRetorno, clientSocket, dest);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    end:
        return status;
}

/*
---------------------------------------
--------   LIST ALL PROFILES   --------
---------------------------------------
*/

bool listAllProfiles(int clientSocket, sockaddr_in dest){

    bool status = true;
    sqlite3 *db;
    char **sql_return;
    char *err_msg = 0;
    int n, linecount, collumncount, i;
    
    int rc = sqlite3_open(server_path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(db));
        sqlite3_close(db);
        
        status = false;
        goto end;
    }
    
    char *sql = "SELECT * FROM Profiles";
    sqlite3_stmt *stmt;

    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    rc = sqlite3_prepare_v2(db, sql, QUERY_MAX_SIZE, &stmt, (const char**)&err_msg);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        status = false;
        goto end;
    }

    n = sqlite3_column_count(stmt);
    if(n > MAX_COLUMN){
        printf("Max collumn count reached\n");
        status = false;
        goto end;
    }

    linecount = 0;
    sql_return = malloc(sizeof(char*)*MAX_LINE*MAX_COLUMN);
    do{
        rc = sqlite3_step(stmt);
        
        if (rc == SQLITE_ROW) {
            for(collumncount = 0; collumncount < n; collumncount++){
                sql_return[n*linecount+collumncount] = malloc(sizeof(char)*MAX_CARAC);
                strcpy(sql_return[n*linecount+collumncount], (const char*)sqlite3_column_text(stmt, collumncount));
            }
        }

        linecount++;
        if(linecount == MAX_ROW){
            printf("Max line count reached\n");
            break;
        }
    }while(rc != SQLITE_DONE);
    
    char* stringRetorno = formatResponse(status, sql_return, linecount-1, n);
    for(i = 0; i < linecount - 1 ; i++){
        for(collumncount = 0; collumncount < n; collumncount++){
            free(sql_return[n*linecount+collumncount]);
        }
    }
    free(sql_return);

    status = sendResponse(stringRetorno, clientSocket, dest);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    end:
        return status;
}

/*
----------------------------------
--------   LIST PROFILE   --------
----------------------------------
*/

bool readProfile(char *email, int clientSocket, sockaddr_in dest){
    bool status = true;
    sqlite3 *db;
    char **sql_return;
    char *err_msg = 0;
    int n, linecount, collumncount, i;
    
    int rc = sqlite3_open(server_path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return false;
    }
    
    char sql[QUERY_MAX_SIZE];
    strcpy(sql, "SELECT * FROM Profiles WHERE Email == '");
    strcat(sql, email);
    strcat(sql, "';");
    sqlite3_stmt *stmt;

    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    rc = sqlite3_prepare_v2(db, sql, QUERY_MAX_SIZE, &stmt, (const char**)&err_msg);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        status = false;
        goto end;
    }

    n = sqlite3_column_count(stmt);
    if(n > MAX_COLUMN){
        printf("Max collumn count reached\n");
        status = false;
        goto end;
    }

    linecount = 0;
    sql_return = malloc(sizeof(char*)*MAX_LINE*MAX_COLUMN);
    do{
        rc = sqlite3_step(stmt);
        
        if (rc == SQLITE_ROW) {
            for(collumncount = 0; collumncount < n; collumncount++){
                sql_return[n*linecount+collumncount] = malloc(sizeof(char)*MAX_CARAC);
                strcpy(sql_return[n*linecount+collumncount], (const char*)sqlite3_column_text(stmt, collumncount));
            }
        }

        linecount++;
        if(linecount == MAX_ROW){
            printf("Max line count reached\n");
            break;
        }
    }while(rc != SQLITE_DONE);
    
    char* stringRetorno = formatResponse(status, sql_return, linecount-1, n);


    for(i = 0; i < linecount - 1 ; i++){
        for(collumncount = 0; collumncount < n; collumncount++){
            free(sql_return[n*linecount+collumncount]);
        }
    }
    free(sql_return);
    status = sendResponse(stringRetorno, clientSocket, dest);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    end:
        return status;
    
    return true;
}

/*
-------------------------------------
--------   REMOVE EDUCATION  --------
-------------------------------------
*/

bool removeProfile(char *email, int clientSocket, sockaddr_in dest){
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(server_path, &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
        sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return false;
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON", callback, 0, &err_msg);
    
    char sql[QUERY_MAX_SIZE];
    strcpy(sql, "DELETE FROM Profiles WHERE Email == '");
    strcat(sql, email);
    strcat(sql, "';");
    
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return false;
    } 
    
    char* response = formatResponse(true, NULL, 0, 0);

    sqlite3_close(db);

    return sendResponse(response, clientSocket, dest);

}

/*
----------------------------------------
--------   AUXILIARY FUNCTIONS  --------
----------------------------------------
*/

//prints query results
int callback(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}

char* formatResponse(bool status, char** data, int lineCount, int collumnCount){
    //Create JSON object
    cJSON* profiles;
    JSONquery query;

    char *string = NULL;
    

    //WRAPPER
    query.wrapper = cJSON_CreateObject();
    if (query.wrapper == NULL){
        goto end;
    }
    
    //OPERATION
    if(status == true){
        query.operation = cJSON_CreateString("SUCCESS");
    }else{
        query.operation = cJSON_CreateString("FAILED");
    }

    if (query.operation == NULL){
        goto end;
    }
    /* after creation was successful, immediately add it to the monitor,
     * thereby transferring ownership of the pointer to it */
    cJSON_AddItemToObject(query.wrapper, "response", query.operation);

    //PROFILES OBJECT
    if(data != NULL){
        //adiciona vetor com stirngs
        profiles = formatProfiles( data, lineCount, collumnCount);
        if (profiles == NULL){
            goto end;
        }
        cJSON_AddItemToObject(query.wrapper, "profiles", profiles);
    }
    
    //JSONfy to string
    string = cJSON_PrintUnformatted(query.wrapper);
    if (string == NULL)
    {
        fprintf(stderr, "Objeto JSON não pôde ser exibido.\n");
    }

    if(0){
        end:
        printf("Falha ao criar o objeto JSON.\n");
    }
    
    cJSON_Delete(query.wrapper);
    return string; 
}

cJSON* formatProfiles(char** data, int lineCout, int collumnCount){
        //Create JSON object
    cJSON* Profiles =  cJSON_CreateArray();
    JSONquery query;
    int line, collumn;
    char campo[15];

    for(line = 0; line < lineCout; line++){

        query.wrapper = cJSON_CreateObject();
        if (query.wrapper == NULL){
            goto end;
        }
        
        
        for(collumn = 0; collumn < collumnCount; collumn++){
            switch(collumn){
                case 0:
                    strcpy(campo, "email");
                    break;
                case 1:
                    strcpy(campo, "name");
                    break;
                case 2:
                    strcpy(campo, "surname");
                    break;
                case 3:
                    strcpy(campo, "address");
                    break;
                case 4:
                    strcpy(campo, "education");
                    break;
                case 5:
                    strcpy(campo, "graduationYear");
                    break;
            }
            query.operation = cJSON_CreateString(data[line*collumnCount+collumn]);
            if (query.operation == NULL){
                goto end;
            }
            /* after creation was successful, immediately add it to the monitor,
            * thereby transferring ownership of the pointer to it */
            cJSON_AddItemToObject(query.wrapper, campo, query.operation);
        }
        if(0){
            end:
            printf("Falha ao criar o objeto JSON.\n");
            break;
        }
        cJSON_AddItemToArray(Profiles, query.wrapper);
    }

    return Profiles;
}
