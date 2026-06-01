#include "header.h"

#define MAX_CLIENTS 100
#define BACKLOG 5

#define handle_error(msg) \
    do{ perror(msg); exit(EXIT_FAILURE); } while(0)

client_info clients[MAX_CLIENTS];

int sfd;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


/*============================================================================================
                   		 	CLIENT INITIALIZATION
==============================================================================================*/

void client_info_init(void){

    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].sfd = 0;
        clients[i].userName[0] = '\0';
        clients[i].active_status = 0;
    }
    
}


/*=============================================================================================
               			       BROADCAST FUNCTION
===============================================================================================*/

void broadcast(char *msg, int sender_fd){

    ssize_t bytes_written;

    pthread_mutex_lock(&clients_mutex);

    for(int i = 0; i < MAX_CLIENTS; i++){

        if(clients[i].sfd != 0 && clients[i].sfd != sender_fd){
            bytes_written = write(clients[i].sfd, msg, strlen(msg) + 1);
            if(bytes_written == -1){
                continue;
            }
        }
        
    }
    pthread_mutex_unlock(&clients_mutex);
}


/*==============================================================================================
                    				REMOVE CLIENT
===============================================================================================*/

void remove_client(int client_fd){

    pthread_mutex_lock(&clients_mutex);

    for(int i = 0; i < MAX_CLIENTS; i++){
        
        if(clients[i].sfd == client_fd){
            clients[i].sfd = 0;
            clients[i].active_status = 0;
            clients[i].userName[0] = '\0';
            break;
        }
        
    }

    pthread_mutex_unlock(&clients_mutex);
}


/*========================================================================================================
                    				GET USERNAME
==========================================================================================================*/

void get_username(int client_fd, char *username){

    pthread_mutex_lock(&clients_mutex);

    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].sfd == client_fd){
            strcpy(username, clients[i].userName);
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}


/*==========================================================================================================
                  		  CLIENT THREAD
============================================================================================================*/

void *handle_thread(void *arg){

    int client_fd = *(int *)arg;

    free(arg);

    char username[50];
    char sender_name[50];
    char msg[512];
    char final_msg[600];


/*=================================================
            		RECEIVE USERNAME
=====================================================*/

    int name_bytes = read(client_fd, username, sizeof(username)-1);

    if(name_bytes <= 0){
        close(client_fd);
        pthread_exit(NULL);
    }

    username[name_bytes] = '\0';


/*=====================================================
          	 STORE USERNAME IN STRUCT
========================================================*/

    pthread_mutex_lock(&clients_mutex);

    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].sfd == client_fd){
            strcpy(clients[i].userName, username);
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);


    /*=========================================
                JOIN MESSAGE
    =========================================*/

    snprintf(final_msg, sizeof(final_msg), "%s joined chat\n",  username);
    broadcast(final_msg, client_fd);



    /*=========================================
                CHAT LOOP
    =========================================*/

    while(1){

        memset(msg, 0, sizeof(msg));
        int bytes_read = read(client_fd, msg, sizeof(msg)-1);


        /*=====================================
                DISCONNECT HANDLING
        =====================================*/

        if(bytes_read <= 0){

            snprintf(final_msg, sizeof(final_msg), "%s left chat\n", username);

            broadcast(final_msg, client_fd);

            remove_client(client_fd);

            close(client_fd);

            pthread_exit(NULL);
        }


        msg[bytes_read] = '\0';



        /*=====================================
                    /quit
        =====================================*/

        if(strcmp(msg, "/quit") == 0){
            snprintf(final_msg, sizeof(final_msg), "%s left chat\n", username);
            
            broadcast(final_msg, client_fd);
            remove_client(client_fd);
            close(client_fd);
            
            pthread_exit(NULL);
        }



        /*=====================================
                NORMAL MESSAGE
        =====================================*/

        get_username(client_fd, sender_name);

        snprintf(final_msg, sizeof(final_msg), "%s : %s", sender_name, msg);

        broadcast(final_msg, client_fd);
    }

    return NULL;
}



/*=========================================================
                        MAIN
=========================================================*/

int main(void){

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sfd == -1){
        handle_error("Socket");
    }


    struct sockaddr_in server;

    memset(&server, 0, sizeof(server));



    server.sin_family = AF_INET;
    server.sin_port = htons(2000);
    server.sin_addr.s_addr = htonl(INADDR_ANY);



    socklen_t ser_size = sizeof(server);



    if(bind(sfd, (struct sockaddr *)&server, ser_size) == -1){
        handle_error("Bind");
    }



    if(listen(sfd,BACKLOG) == -1){
        handle_error("Listen");
    }



    signal(SIGPIPE, SIG_IGN);

    client_info_init();


    printf("Server started...\n");



    /*=========================================
                ACCEPT LOOP
    =========================================*/

    while(1){

        struct sockaddr_in client_detail;

        socklen_t cli_size =
            sizeof(client_detail);

        int new_fd = accept(sfd, (struct sockaddr *)&client_detail, &cli_size);

        if(new_fd == -1){
            handle_error("Accept");
        }

        printf("Client Connected\n");

        int *pclient = malloc(sizeof(int));

        *pclient = new_fd;


/*====================================================================================================
           				     STORE CLIENT
=====================================================================================================*/

        pthread_mutex_lock(&clients_mutex);

        for(int i = 0; i < MAX_CLIENTS; i++){

            if(clients[i].active_status == 0){
                clients[i].sfd = new_fd;
                clients[i].active_status = 1;
                break;
            }
            
        }

        pthread_mutex_unlock(&clients_mutex);


/*=============================================================================================================
              				  CREATE THREAD
===============================================================================================================*/

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_thread, pclient);
        pthread_detach(client_thread);
    }

    return 0;
}
