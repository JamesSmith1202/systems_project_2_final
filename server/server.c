#include "server.h"
#include "../include/protocol.h"

int max_fd = 0;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

//scans the server socket for a connection, accepts it, and adds that fd to the idle room.
void scan_accept(int listen_sock, struct sockaddr_storage *client_addr , socklen_t *client_addr_size, struct timeval timeout, struct chat_room * idle){
  int new_fd;
  int status;
  char s[INET6_ADDRSTRLEN]; 
  fd_set * set;
  FD_ZERO(set);
  FD_SET(listen_sock, set);//add the listening socket to the set
  status = select(max_fd + 1, set, NULL, NULL, timeout);//check for client connections
  if(status == -1){
    perror("select");
    exit(1);
  }
  if(status > 0){//if there is a connection waiting...
    int i;
    new_fd = accept(listenSocketfd, (struct sockaddr *)client_addr, client_addr_size); //accept new connection and save the socket fd for this connection
    if (new_fd == -1) //check for error
	{
	  perror("accept");
	}
    else{
        is_max(new_fd);
        FD_SET(new_fd, idle->users)//add the new connection to the idle room
        idle->num_users++;
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof(s));//convert to human form
        printf("Connection received from %s\n", s);
    }
  }
}

//scans a room for input and handles their message
void scan_room(struct timeval timeout, struct chat_room * room){
    int i = 0;
    int n;//number of bytes received
    struct client_message buff;//buffer for client messages 
    fd_set read_fds = *(room->users);//setting them equal to each other copies users into read_fds
    if((select(max_fd+1, &read_fds, NULL, NULL, timeout)) == -1){
        perror("select");
        exit(1);
    }
    for(;i<=max_fd; i++){//iterate through all fds that the program has
        if(FD_ISSET(i, &read_fds)){//if i is part of this set...
            if ((n = recv(i, buff, sizeof(buf), 0)) < 1) {//if error or closed connection...
                if (n == 0) {//closed connection
                    printf("socket %d disconnected\n", i);
                } 
                else{
                   perror("recv");
                }
                close(i); // close the connection because we dont like those who send errors
                FD_CLR(i, room->users); // remove from master set
                room->num_users--;
            }
            else{//we have data
                handle_message(i, buff, room);
            }
        }
    }
}

//handles a client's message according to protocol
void handle_message(int client_fd, struct client_message msg, struct chat_room * room){

}

void is_max(int i){
    if(i > max_fd){
        max_fd = i;
    }
}

int main()
{
    printf("Starting server setup...\n");

    int listenSocketfd;
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo; // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the structure is empty to fill with info
    hints.ai_family = AF_UNSPEC;     // We dont care if the address is IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // Automatically assign the address of my local host to the socket structures

    if ((status = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) //if there is an error with allocating and initializing the linked list of addrinfo structures, then return an error message and exit
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    //servinfo is now a pointer to a linked list of addrinfo structs

    //loop through the linked list and bind to the first working addrinfo struct
    struct addrinfo *p;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {

        //int socket(int domain, int type, int protocol) returns the socket descriptor to be used in later system calls
        if ((listenSocketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        { //if there is an issue setting up a socket, print the error and exit
            perror("server: socket");
            continue;
        }
        int optval = 1;
        if (setsockopt(listenSocketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) //allows bind() to reuse local addresses
        {
            perror("setsockopt");
            exit(1);
        }
        // int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
        if ((status = bind(listenSocketfd, servinfo->ai_addr, servinfo->ai_addrlen)) != 0)
        { //if there is an error with binding the socket to a port on the host computer, print the error and exit
            close(listenSocketfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    //Below this point servinfo is of no use so free the linked list from memory -----------------------------------------------------------------------
    freeaddrinfo(servinfo);

    //int listen(int sockfd, int backlog);
    if ((status = listen(listenSocketfd, BACKLOG)) != 0)
    { //if there is an issue listening on a specific port, print the error and exit
        perror("listen");
        exit(1);
    }

    socklen_t lenSockAddr = sizeof(struct sockaddr);
    struct sockaddr_in foo;
    getsockname(listenSocketfd, (struct sockaddr *)&foo, &lenSockAddr);
    fprintf(stderr, "Server is listening on %s:%d\n", inet_ntoa(foo.sin_addr), ntohs(foo.sin_port));

    //main accept loop
    struct sockaddr_storage client_addr; //connector's address information
    socklen_t client_addr_size;          //size of the peer
    int new_fd;                          //new connection put on new_fd
    client_addr_size = sizeof(client_addr);

    struct timeval timeout;//set timeout vals to 0 so it polls instead of waiting in select for connections
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    max_fd = listenSocketfd;//need to keep track of largest fd for select

    Array chatrooms;//Array is typedef
    init_arr(&chatrooms, MIN_ROOMS);//initialize the chatrooms resizable array

    struct chat_room idle;//a room where new connections/fds with no chat room go. just listens for commands.
    idle.name = "IDLE";
    idle.num_users = 0;
    insert(&chatrooms, idle);//idle will be at the 0 index

    int i;
    while (true){ //infinite serving loop
      scan_select(listenSocketfd, &client_addr, &client_addr_size, timeout, &idle);//scan for new connections
      for(i=0;i<chatrooms.len;i++){//iterate through chatrooms
          scan_room(timeout, chatrooms.array[i]);//handle each individual chatroom
      }
    }
    return 0;
}
