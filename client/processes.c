#include<cdk/cdk.h>

#include"../include/protocol.h"
#include"../include/parse.h"
#include"networking.h"
#include"processes.h"

//TODO a stretch, but factor out the network write calls to a single function

//used by main process to print data from network
void network_print(char *data, CDKSWINDOW *scroll, CDKSCREEN *screen) {
	char *tok = (char *)malloc(128);
	
	while( (tok = strsep(&data, "\n")) != 0) {
		if (tok[0] != '\n') addCDKSwindow(scroll, tok, BOTTOM);
	}
	
	refreshCDKScreen(screen);
	
	free(tok);
	tok = 0;
}

void graphics_process(int read_cursor_fd, int read_network_fd) {
		//int read_network_fd, int write_network_fd) {
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
	
	char s[MSG_MAX_LEN];
	memset(s, 0, sizeof(s));
	int cur_x, cur_y;
	move(max_y-2, 1);
	getyx(stdscr, cur_y, cur_x);
	refresh();
	refreshCDKScreen(log_screen);
	while(1) {
		memset(s, 0, sizeof(s));
		if (read(read_cursor_fd, s, sizeof(s)) != -1) {
			if (s[0] == '\n'/*strlen(s) > 1*/) {
				//addCDKSwindow(scroll, s, BOTTOM);
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
		}
		refresh();
	}
	
	destroyCDKScreen(log_screen);
	destroyCDKScreen(input_screen);
	endCDK();
}

void input_process(int write_cursor_fd, int write_input_fd) {
	int c;
	char str_buf[MSG_MAX_LEN];
	int index = 0;
	int max_x, max_y;
	
	getmaxyx(stdscr, max_y, max_x);
	
	while( (c = getch()) /*== some quit key*/ ) {
		switch(c) {
		case '\n':
			str_buf[index++] = '\n';
			str_buf[index] = 0;
			index = 0;
			write(write_cursor_fd, &c, sizeof(char));
			if (strlen(str_buf) > 1) {
				write(write_input_fd, str_buf, strlen(str_buf));
			}
			memset(str_buf, 0, sizeof(str_buf));
		break;
		
		case 127:
		case KEY_BACKSPACE:
			if (index > 0) str_buf[--index] = 0;
			write(write_cursor_fd, &c, sizeof(char));
		break;
		
		default:
			if (index < max_x-2) {
				str_buf[index++] = c;
				write(write_cursor_fd, &c, sizeof(char));
			}
		break;
		}
	}
}

/*
ask user for ip/port to connect to
	keep asking for this if connection fails
ask user for username
	keep asking if username already exists
ask user which chatroom to join
	keep asking if chatroom doesn't exist
user can begin sending messages

network process will package message into the protocol
network process will handle response message
*/
void network_process(int read_fd, int write_fd) {
	char message[MSG_MAX_LEN];
	
	struct addrinfo hint, *data;
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	
	char ip[64];
	char port[16];
	do {
		memset(ip, 0, sizeof(ip));
		memset(port, 0, sizeof(port));
		
		sprintf(message, "Please enter an ip address\n");
		write(write_fd, message, strlen(message));
		read(read_fd, ip, sizeof(ip));
		if (strchr(ip, '\n') != 0) *strchr(ip, '\n') = 0;

		sprintf(message, "Now enter the port\n");
		write(write_fd, message, strlen(message));
		read(read_fd, port, sizeof(port));
		if (strchr(port, '\n') != 0) *strchr(port, '\n') = 0;
		
		sprintf(message, "Checking for validity: %s:%s\n", ip, port);
		write(write_fd, message, strlen(message));
	}
	while(!valid_connection(write_fd, hint, &data, ip, port));
	
	//debugging, print all returned addresses
	print_addr_list(write_fd, data);
	
	int my_fd = socket(data->ai_family, data->ai_socktype, data->ai_protocol);
	//if (my_fd == -1)
	sprintf(message, "Created socket with fd: %d\n", my_fd);
	write(write_fd, message, strlen(message));
	
	if (connect(my_fd, data->ai_addr, data->ai_addrlen) == -1) {
		sprintf(message, "Connection failed\n");
		write(write_fd, message, strlen(message));
		//better handling
	}
	else {
		sprintf(message, "Connection success\n");
		write(write_fd, message, strlen(message));
	}
	
	int bytes;
	memset(message, 0, sizeof(message));
	//wait for user input data to come in
	while( read(read_fd, message, sizeof(message)) != -1 ) {
		bytes = send(my_fd, message, strlen(message), 0);
		if (bytes == -1) {
			sprintf(message, "Write failed\n");
			write(write_fd, message, strlen(message));
		}
		bytes = recv(my_fd, message, sizeof(message), 0);
		write(write_fd, message, strlen(message));
		memset(message, 0, sizeof(message));
	}
}

