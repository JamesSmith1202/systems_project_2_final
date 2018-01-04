#ifndef PROCESSES_H
#define PROCESSES_H

#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/types.h>

//fd's for pipes
#define FD_READ		0
#define FD_WRITE	1

/*
Program flow:

input process will transmit currently entered chars to main (graphics) process
when buffer flushed, main will pass data to network process
network process will communicate with server
network process will receive data from server
network process sends data to main process
main process executes any neccesary routines

main process will ONLY update chat log when it RECEIVES from network process

main process's tasks:
update cursor
update chat log
receive data from input
send data to network
receive data from network

input process's tasks:
listen for user input
send input to main process

network process's tasks:
package data (as per the protocol)
send data to server
receive data from server
unpackage data?
receive data from main
send data to main


*/

void graphics_process(int read_input_fd, int read_network_fd, int write_network_fd);

void input_process(int write_fd);

void network_process(int read_fd, int write_fd);

#endif
