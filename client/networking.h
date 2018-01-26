#ifndef NETWORKING_H
#define NETWORKING_H

#include"../include/protocol.h"

#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>

//prints each address returned from getaddrinfo
void print_addr_list(int write_fd, struct addrinfo *data);

//tests for a valid ip/port pair
int valid_connection(int write_fd, struct addrinfo hint, struct addrinfo **data,
		char *ip, char *port);

//packs client message as per the protocol
void pack_message(struct client_message *outgoing, char *msg,
	char *username, char *chatroom, short *disconnect);

//packs message from server as per the protocol
void unpack_message(struct server_message *incoming, char *msg, short *in_room);

#endif
