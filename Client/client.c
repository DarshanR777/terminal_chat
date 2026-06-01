#include "header.h"

int sfd;

void handle_SIGINT(int sig){
	write(sfd, "/quit", sizeof("/quit"));
	shutdown(sfd, SHUT_RDWR);
	sleep(1);
	close(sfd);
	_exit(0);
}

void *receive(void *arg){
		char recv[100];
		int read_bytes = 0;
		while(1){
			memset(&recv, 0, sizeof(recv));
			read_bytes = read(sfd, recv, sizeof(recv)-1);
			
			if(read_bytes <= 0){
				recv[read_bytes] = '\0';
				shutdown(sfd, SHUT_RDWR);
    				close(sfd);
   				exit(0);
			}
			recv[read_bytes] = '\0';
			printf("%s",recv);
		}	
}

void *send_data(void *arg){
	char snd[100];
	int write_bytes = 0;
	while(1){
		memset(&snd, 0, sizeof(snd));
		if(fgets(snd,sizeof(snd) ,stdin) == NULL){
			close(sfd);
			return NULL;
		}
		write_bytes = write(sfd,snd,strlen(snd)+1);
		
		if( write_bytes == -1){
			printf("Transmission failed\n");
			
			shutdown(sfd, SHUT_RDWR);
    			close(sfd);
   			exit(0);
   			
			return NULL;
		}
	}

}

int main(void){
	int len; //Socket ID

	// Creating a Buffer to make a connection between server and client
	sfd = socket(AF_INET,SOCK_STREAM,0);//int socket(int domain, int type, int protocol);

	if(sfd<0){
		perror("Socket");
		return -1;
	}

/*====================================== Connect ============================================*/
	
	struct sockaddr_in serverId, clientId;//structure (man 7 ip)

	serverId.sin_family = AF_INET; //IPv4
	serverId.sin_port =  htons(2000); //(man htons) to understand byte order

	serverId.sin_addr.s_addr = inet_addr("192.168.0.6");
	len=sizeof(serverId);
	
	if(connect(sfd,(struct sockaddr*)&serverId,len)<0){ // int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
		perror("Connect");
		return 0;
	}
	
	signal(SIGINT, handle_SIGINT);
	
	char username[50];
	puts("Enter the user name (only alpha numeric keys are allowed)");
	
	fgets(username, sizeof(username), stdin);
	username[strcspn(username, "\n")] = '\0';
	
	write(sfd, username, strlen(username)+1);
	
	pthread_t tx,rx;
	pthread_create(&tx,NULL,send_data, NULL);
	pthread_create(&rx, NULL, receive, NULL);
	pthread_join(tx, NULL);
	pthread_join(rx, NULL);

	
	perror("Connect");
	
}


