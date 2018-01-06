#include<cdk/cdk.h>

#include"processes.h"

int main() {
	//pipe fd's
	int cursor[2], input[2], network[2];
	pipe(cursor);
	pipe(network);
	pipe(input);
	
	//the only pipe we want to block is the input to network pipe
	//all other ones will be nonblocking
	int flags = fcntl(network[FD_READ], F_GETFL, 0);
	fcntl(network[FD_READ], F_SETFL, flags | O_NONBLOCK);
	
	flags = fcntl(cursor[FD_READ], F_GETFL, 0);
	fcntl(cursor[FD_READ], F_SETFL, flags | O_NONBLOCK);
	
	int is_parent = fork();
	if (is_parent) {
		//initialize ncurses here cuz network won't need it
		initscr();
		noecho();
		cbreak();
		
		is_parent = fork();
		
		if (is_parent) {
			graphics_process(cursor[FD_READ], network[FD_READ]);
		}
		else {
			input_process(cursor[FD_WRITE], input[FD_WRITE]);
		}
	}
	else {
		network_process(input[FD_READ], network[FD_WRITE]);
	}
	
	return 0;
}
