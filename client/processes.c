#include<cdk/cdk.h>

#include"../include/protocol.h"
#include"networking.h"
#include"processes.h"

//used by main process to print data from network
void network_print(char *data, CDKSWINDOW *scroll, CDKSCREEN *screen) {
	char *tok = (char *)malloc(128);
	//char *temp = (char *)malloc(256);
	//strncpy(temp, data, sizeof(temp));
	
	while( (tok = strsep(&data, "\n")) != 0) {
		//tok = strsep(&temp, "\n");
		if (strlen(tok) > 1) addCDKSwindow(scroll, tok, BOTTOM);
	}
	
	refreshCDKScreen(screen);
	
	free(tok);
	tok = 0;
}

void graphics_process(int read_input_fd,
		int read_network_fd, int write_network_fd) {
	WINDOW *input, *log;
	int max_x, max_y;
	
	getmaxyx(stdscr, max_y, max_x);
	
	//not const, as I might implement resizing later
	int	  input_height	= 3
		, input_width	= max_x;
	
	int	  log_height	= max_y - input_height
		, log_width	= max_x;
	
	//height, width, y, x
	log = newwin(log_height, log_width, 0, 0);
	input = newwin(input_height, input_width, max_y-3, 0);
	
	box(input, 0, 0);
	
	
	refresh();
	wrefresh(input);
	wrefresh(log);
	

	CDKSCREEN *log_screen;
	CDKSCREEN *input_screen;
	CDKSWINDOW *scroll;
	
	log_screen = initCDKScreen(log);
	input_screen = initCDKScreen(input);
	
	scroll = newCDKSwindow(
		log_screen,
		1, 1,
		log_height-2, log_width,
		"Chat log",
		18,
		1,
		0
	);
	
	char s[128];
	memset(s, 0, sizeof(s));
	int cur_x, cur_y;
	move(max_y-2, 1);
	getyx(stdscr, cur_y, cur_x);
	refresh();
	refreshCDKScreen(log_screen);
	//wrefresh(input);
	//wrefresh(log);
	while(1) {
		memset(s, 0, sizeof(s));
		if (read(read_input_fd, s, sizeof(s)) != -1) {
			if (strlen(s) > 1) {
				//mvwprintw(log, max_y-4, 1, "");
				addCDKSwindow(scroll, s, BOTTOM);
				while (cur_x > 1) {
					move(cur_y, --cur_x);
					addch(' ');
					move(cur_y, cur_x);
				}
				//refreshCDKScreen(log_screen);
			}
			//else if (s[0] == KEY_BACKSPACE) {
			else if (s[0] == 127) {
				if (cur_x > 1) {
					move(cur_y, --cur_x);
				}
				addch(' ');
				move(cur_y, cur_x);
				
			}
			
			else if (s[0] != '\n') {
				addch(s[0]);
				cur_x++;
			}
		}
		
		memset(s, 0, sizeof(s));
		if (read(read_network_fd, s, sizeof(s)) != -1) {
			
			network_print(s, scroll, log_screen);
			
			/*
			addCDKSwindow(scroll, s, BOTTOM);
			refreshCDKScreen(log_screen);
			*/
		}
		refresh();
	}
	
	destroyCDKScreen(log_screen);
	destroyCDKScreen(input_screen);
	endCDK();
}

void input_process(int write_fd) {
	int c;
	char str_buf[128];
	int index = 0;
	int max_x, max_y;
	
	getmaxyx(stdscr, max_y, max_x);
	
	while( (c = getch()) /*== some quit key*/ ) {
		switch(c) {
		case '\n':
			str_buf[index++] = '\n';
			str_buf[index] = 0;
			index = 0;
			write(write_fd, str_buf,
				strlen(str_buf)*sizeof(char));
		break;
		
		case 127:
		case KEY_BACKSPACE:
			if (index > 0) str_buf[--index] = 0;
			write(write_fd, &c, sizeof(char));
		break;
		
		default:
			if (index < max_x-2) {
				str_buf[index++] = c;
				write(write_fd, &c, sizeof(char));
			}
		break;
		}
	}
}

void network_process(int read_fd, int write_fd) {
	char message[256];
	
	struct addrinfo hint, *data;
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	
	//should pass in the server's address
	if (getaddrinfo(0, "12321", &hint, &data) == -1) {
		//send message to main that error occured
		strncpy(message, "An error occured when connecting",
			sizeof(message));
		write(write_fd, message, strlen(message));
	}
	
	//debugging, print all returned addresses
	print_addr_list(write_fd, data);
	
	int my_fd = socket(data->ai_family, data->ai_socktype, data->ai_protocol);
	//if (my_fd == -1)
	sprintf(message, "Created socket with fd: %d\n", my_fd);
	write(write_fd, message, strlen(message));
}

