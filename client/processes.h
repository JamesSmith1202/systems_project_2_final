#ifndef PROCESSES_H
#define PROCESSES_H

#include<unistd.h>
#include<fcntl.h>

//fd's for pipes
#define FD_READ		0
#define FD_WRITE	1

//max length of message
#define MSG_MAX_LEN	256

/*
Program flow:

input process will transmit currently entered chars to main (graphics) process
when buffer flushed, main will clear input line
input will send data to network
network process will communicate with server
network process will receive data from server
network process sends data to main process
main process executes any neccesary routines

main process will ONLY update chat log when it RECEIVES from network process

main process's tasks:
update cursor
update chat log
receive chars from input
send data to network
receive data from network

input process's tasks:
listen for user input
send chars to main process
send data to network

network process's tasks:
package data (as per the protocol)
send data to server
receive data from server
unpackage data?
receive data from input
send data to main


*/

//read from network
void graphics_process(int read_cursor_fd, int read_network_fd);

//read from input, write to main
void network_process(int read_fd, int write_fd);

//write to network, write individual chars to graphics (to update cursor)
void input_process(int write_cursor_fd, int write_input_fd);

#endif
