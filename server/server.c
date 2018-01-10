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
    struct sockaddr_storage client_addr; //connector's address information
    socklen_t client_addr_size;          //size of the peer
    int new_fd;                          //new connection put on new_fd
    char s[INET6_ADDRSTRLEN];            // string that is the length of an ipv6 address
    client_addr_size = sizeof(client_addr);
    
    while (true) //infinite serving loop
    {
        new_fd = accept(listenSocketfd, (struct sockaddr *)&client_addr, &client_addr_size); //accept new connection and save the socket fd for this connection
        if (new_fd == -1) //check for error
        {
            perror("accept");
            continue;
        }
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof(s));
        printf("Connection received from %s\n", s);
    }
    return 0;
}
