#include<cdk/cdk.h>

#include"processes.h"

int main() {
	//pipe fd's
	//upstream to main (main reads from it)
	int input[2], network_upstream[2], network_downstream[2];
	pipe(input);
	pipe(network_upstream);
	pipe(network_downstream);
	
	//set flags so reading is nonblocking
	int flags = fcntl(input[FD_READ], F_GETFL, 0);
	fcntl(input[FD_READ], F_SETFL, flags | O_NONBLOCK);
	
	flags = fcntl(network_upstream[FD_READ], F_GETFL, 0);
	fcntl(network_upstream[FD_READ], F_SETFL, flags | O_NONBLOCK);
	
	flags = fcntl(network_downstream[FD_READ], F_GETFL, 0);
	fcntl(network_downstream[FD_READ], F_SETFL, flags | O_NONBLOCK);
	
	int is_parent = fork();
	if (is_parent) {
		//initialize ncurses here cuz network won't need it
		initscr();
		noecho();
		cbreak();
		
		is_parent = fork();
		
		if (is_parent) {
			graphics_process(input[FD_READ],
				network_upstream[FD_READ],
				network_downstream[FD_WRITE]);
		}
		else {
			input_process(input[FD_WRITE]);
		}
	}
	else {
		network_process(network_downstream[FD_READ],
				network_upstream[FD_WRITE]);

	}
	
	return 0;
}
