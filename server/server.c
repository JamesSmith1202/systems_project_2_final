#include "server.h"
#include "protocol.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int sendHelper(int sockfd, const char *msg) //streamlines the sending
{
    int stat = send(sockfd, msg, strlen(msg), 0);
    if (stat == -1)
    {
        perror("send");
    }
    else
    {
        printf("%d bytes of %lu total were sent through socketfd #%d\n", stat, strlen(msg), sockfd);
    }
    return stat;
}

//scans the server socket for connections and accepts them -- PROBLEM: NO WAY TO RETURN ACCEPTED SOCKET FD'S!
void scan_accept(int listen_sock, fd_set *set, struct sockaddr_storage *client_addr , socklen_t *client_addr_size){
  int new_fd;
  int status;
  char s[INET6_ADDRSTRLEN]; 
  FD_ZERO(set);
  FD_SET(listen_sock, set);//add the listening socket to the set
  status = select(listen_sock + 1, set, NULL, NULL, NULL);//check for client connections
  if(status == -1){
    perror("select");
  }
  if(status > 0){//if there is at least 1...
    int i;
    for(i = status; i > 0; i--){//iterate through until there are 0 left
      new_fd = accept(listenSocketfd, (struct sockaddr *)client_addr, client_addr_size); //accept new connection and save the socket fd for this connection
      if (new_fd == -1) //check for error
	{
	  perror("accept");
	}
      inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof(s));//convert to human form
      printf("Connection received from %s\n", s);
    }
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

    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

    //main accept loop
    fd_set accept_set;
    struct sockaddr_storage client_addr; //connector's address information
    socklen_t client_addr_size;          //size of the peer
    int new_fd;                          //new connection put on new_fd
    client_addr_size = sizeof(client_addr);
    struct chat_room room;
    while (true) //infinite serving loop
    {
      scan_select(listenSocketfd, &accept_set, &client_addr, &client_addr_size);
    }
    return 0;
}
