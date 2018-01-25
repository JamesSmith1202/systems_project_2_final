#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "array.h"

#define MYPORT "3490"   //Port to listen on
#define BACKLOG 1000    //number of connections allowed on the incoming queue
#define MIN_ROOMS 20

void *get_in_addr(struct sockaddr *);

void scan_accept(int, struct sockaddr_storage *, socklen_t *, struct timeval, struct chat_room *);

void scan_room(struct timeval, struct chat_room *, Array *);

void handle_message(int, struct client_message, struct chat_room *, Array *);

int pack_msg(struct server_message *, unsigned short, char *, char *, short);

struct chat_room * find_room(char *, Array *);

void is_max(int);

#endif