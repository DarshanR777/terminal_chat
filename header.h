#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<string.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<sys/un.h>
#include <netinet/in.h>
#include<signal.h>
#include<errno.h>
#include<pthread.h>

typedef struct {
	int sfd;
	char userName[50];
	int active_status;
}client_info;
