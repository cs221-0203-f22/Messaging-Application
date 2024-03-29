#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#include "project05.h"

#define BUF_SIZE 1000

struct user {
     char status[BUF_SIZE]; 
     char name[BUF_SIZE];  
     char port[BUF_SIZE];  
     char host[NI_MAXHOST];
 };

struct user usersetup(char *name, char* port, char *host) {
 	struct user nextuser;
 	strcpy(nextuser.name,name);
 	strcpy(nextuser.port,port);
 	strcpy(nextuser.host,host);
 	return nextuser;
 }
 
int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("usage: ./project05 username port\n");
		exit(-1);
	}

	char *username = argv[1];
	char *port = argv[2];
	char *classport = "8221";
	int socketfd;
	
	struct addrinfo hints;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_UDP;

	struct addrinfo *result;
	int e = getaddrinfo(NULL, classport, &hints, &result);
	if(e != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(e));
		exit(EXIT_FAILURE);
	}

	struct addrinfo *r;
	for(r = result; r != NULL; r = r->ai_next) {
		int setreuseaddr = 1;
		int setbroadcast = 1;
		socketfd = socket(r->ai_family, r->ai_socktype,r->ai_protocol);
		if (socketfd == -1)
			continue;

		if(setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&setreuseaddr,sizeof(int)) != 0) {
			perror("reuseaddr");
			exit(-1);
		}

		if(setsockopt(socketfd,SOL_SOCKET,SO_BROADCAST, &setbroadcast, sizeof(int)) != 0) {
			perror("broadcast");
			exit(-1);
		}
		
		if(bind(socketfd,r->ai_addr,r->ai_addrlen) != 0) {
			continue;
		}

		break;
		}

	freeaddrinfo(result);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = PF_INET;
	addr.sin_port = htons (8221);
	inet_pton(PF_INET, "10.10.13.255", &addr.sin_addr);

//	char buf[BUF_SIZE];
	struct sockaddr_storage stg;
	memset(&addr, 0, sizeof(struct sockaddr_storage));
//	ssize_t recvfrom(classport, buf ,0, (struct sockaddr *) &stg, &std_len);

	char *presmsg = "gsphicas online 8074";
	int udpsocket = sendto(socketfd,presmsg,strlen(presmsg) + 1,0,(struct sockaddr*) &addr, sizeof(struct sockaddr_in));
	
	bool eof = false;
	struct pollfd my_polls[NUM_POLLS];
	int num_polls = 0;
	int num_readable = 0;

	my_polls[0].fd = STDIN_FILENO;
	my_polls[0].events = POLLIN;
	my_polls[0].revents = 0;
	num_polls++;

	my_polls[1].fd = udpsocket;
	my_polls[1].events = POLLIN;

	char buf[10];
	int sendbroadcast = 0;
	
	while(!eof) {
		int num_readable = poll(my_polls, num_polls, TIMEOUT);

		if(sendbroadcast == 100) {
			sendto(socketfd,presmsg,strlen(presmsg),0,(struct sockaddr*) &addr, sizeof(struct sockaddr_in));
			sendbroadcast = 0;
		}
		sendbroadcast++;
	
		
		if(num_readable > 0) {
			if(my_polls[0].revents & POLLIN) {
				int s = read(my_polls[0].fd, buf,sizeof(buf));
				if(s == -1) {
					printf("read");
					exit(-1);
				}
				printf("%.*s\n",  (int) s, buf);
			} else {
				printf("closing fd %d\n", my_polls[0].fd);
			}

		}

	} 
		return 0;
}
