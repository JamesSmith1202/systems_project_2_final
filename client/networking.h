#ifndef NETWORKING_H
#define NETWORKING_H

#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>

void print_addr_list(int write_fd, struct addrinfo *data);

#endif
