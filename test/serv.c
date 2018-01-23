#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>

#include"../include/protocol.h"

void err(int x) {
	if (x == -1) {
		printf("errno %d: %s\n", errno, strerror(errno));
		exit(1);
	}
}

int main() {
	struct addrinfo *data, hint;
	memset(&hint, 0, sizeof(struct addrinfo));
	
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	
	err( getaddrinfo(0, "49152", &hint, &data) );
	
	printf("%d\n", data->ai_addrlen);
	//printf("%s\n", data->ai_canonname);
	
	char /*res[INET_ADDRSTRLEN],*/ *ipv4;
	char *res = malloc(INET_ADDRSTRLEN);
	struct sockaddr_in *ip = (struct sockaddr_in *)data->ai_addr;
	
	inet_ntop(AF_INET, &(ip->sin_addr), res, data->ai_addrlen);
	//inet_ntop(AF_INET, data->ai_addr->sa_data, res, data->ai_addrlen);
	
	printf("%s\n", res);
	
	int sockfd = socket(data->ai_family,
		data->ai_socktype, data->ai_protocol);
	
	printf("fd: %d\n", sockfd);
	
	err( bind(sockfd, data->ai_addr, data->ai_addrlen) );
	
	err( listen(sockfd, 10) );
	
	struct sockaddr_storage incoming;
	socklen_t len = sizeof(struct sockaddr_storage);
	int in_fd;
	
	err( (in_fd = accept(sockfd, (struct sockaddr *)&incoming, &len)) );
	printf("accept fd: %d\n", in_fd);
	
	/*
	char buf[256];
	int amt_read;
	memset(buf, 0, sizeof(buf));
	while( (amt_read = recv(in_fd, buf, 256, 0)) != -1) {
		buf[0] = '@';
		printf("buf: %s", buf);
		amt_read = send(in_fd, buf, 256, 0);
		err(amt_read);
		memset(buf, 0, sizeof(buf));
	}
	*/
	
	/*
	char message[32];
	strncpy(message, "server", 7);
	*/
	
	struct server_message message;
	strncpy(message.username, "1234567890123456789012345678901Q",
		strlen("1234567890123456789012345678901Q"));
	strncpy(message.message, "How do you do?\n", strlen("How do you do?\n"));
	
	int counter = 0;
	
	struct client_message in;
	struct server_message out;
	while(1) {
		//err( send(in_fd, message, 32, 0) );
		
		if (recv(in_fd, &in, sizeof(in), MSG_DONTWAIT) != -1) {
			printf("got user message\n");
			out.message_type = in.message_type;
			strncpy(out.username, in.username, strlen(in.username));
			strncpy(out.message, in.message, strlen(in.message));
			
			err( send(in_fd, &out, sizeof(out), 0) );
		}
		
		switch (counter) {
			case 0:
				message.message_type = MT_COMMAND;
			break;
			case 1:
				message.message_type = MT_MESSAGE;
			break;
			case 2:
				message.message_type = MT_ERR;
			break;
		}
		err( send(in_fd, &message, sizeof(message), 0) );
		printf("message sent\n");
		
		counter++;
		counter = counter % 3;
		sleep(3);
	}
	
	return 0;
}
