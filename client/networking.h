#ifndef NETWORKING_H
#define NETWORKING_H

#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>

//max length of message
#define MSG_MAX_LEN	256

void print_addr_list(int write_fd, struct addrinfo *data);
int valid_connection(int write_fd, struct addrinfo hint, struct addrinfo **data,
		char *ip, char *port);

#endif
