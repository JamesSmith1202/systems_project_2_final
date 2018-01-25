#include<cdk.h>

#include"../include/protocol.h"
#include"networking.h"
#include"processes.h"

//TODO actually limit input bounds
//TODO a stretch, but factor out the network write calls to a single function

//used by main process to print data from network
void network_print(char *data, CDKSWINDOW *scroll, CDKSCREEN *screen) {
	char *tok;
	
	while( (tok = strsep(&data, "\n")) != 0) {
		if (tok[0] != '\n' && strcmp(tok, "")) addCDKSwindow(scroll, tok, BOTTOM);
	}
	
	refreshCDKScreen(screen);
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
		log_height-1, log_width,
		0,
		log_height-2,
		1,
		0
	);
	
	char network_input[SERVER_MAX_LEN*2];
	int cursor_input;
	//memset(s, 0, sizeof(s));
	int cur_x, cur_y;
	move(max_y-2, 1);
	getyx(stdscr, cur_y, cur_x);
	refresh();
	refreshCDKScreen(log_screen);
	
	//create a set of fds
	fd_set input_set;
	while(1) {
		memset(network_input, 0, sizeof(network_input));
		cursor_input = 0;
		
		FD_ZERO(&input_set);
		FD_SET(read_network_fd, &input_set);
		FD_SET(read_cursor_fd, &input_set);
		
		//a requirement of the select function
		int max = (read_network_fd > read_cursor_fd) ? read_network_fd : read_cursor_fd;
		select(max + 1, &input_set, 0, 0, 0);
		
		if (FD_ISSET(read_cursor_fd, &input_set)) {
			if (read(read_cursor_fd, &cursor_input,
					sizeof(cursor_input)) != -1) {
				/*
				if (cursor_input == KEY_F(2)) {
					break;
				}
				*/
				
				if (cursor_input == '\n') {
					while (cur_x > 1) {
						move(cur_y, --cur_x);
						addch(' ');
						move(cur_y, cur_x);
					}
				}
				//else if (cursor_input == KEY_BACKSPACE) {
				else if (cursor_input == 127) {
					if (cur_x > 1) {
						move(cur_y, --cur_x);
					}
					addch(' ');
					move(cur_y, cur_x);
					
				}
				
				else if (cursor_input != '\n') {
					addch(cursor_input);
					cur_x++;
				}
			}
		}
		else if (FD_ISSET(read_network_fd, &input_set)) {
			if (read(read_network_fd, network_input,
					sizeof(network_input)) != -1) {
				network_print(network_input, scroll, log_screen);
			}
			//refreshCDKScreen(input_screen);
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
	
	while( (c = getch()) /*!= KEY_F(2)*/ ) {
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
			if (index < max_x-2 && index < MSG_MAX_LEN) {
				/*if (isprint(c))*/ str_buf[index++] = c;
				write(write_cursor_fd, &c, sizeof(char));
			}
		break;
		}
	}
	
	/*
	write(write_cursor_fd, &c, sizeof(char));
	
	//write EOT (value 4) to network process
	memset(str_buf, 0, sizeof(str_buf));
	str_buf[0] = 4;
	str_buf[1] = 0;
	write(write_input_fd, str_buf, 2);
	endwin();
	*/
}

//my_fd is the socket
//chatroom is the room the user is currently in
//message is the message array from the network loop
void message_loop(int read_fd, int write_fd, int my_fd,
		char *message, char* username, char *chatroom) {
	int bytes;
	struct client_message outgoing;
	struct server_message incoming;
	short in_room;
	
	fd_set readfds;
	
	while(1) {
		memset(&outgoing, 0, sizeof(outgoing));
		memset(&incoming, 0, sizeof(incoming));
		memset(message, 0, sizeof(message));
		
		FD_ZERO(&readfds);
		FD_SET(read_fd, &readfds);
		FD_SET(my_fd, &readfds);
		
		int max = (read_fd > my_fd) ? read_fd : my_fd;
		
		//wait for either socket read data or user input data
		select(max + 1, &readfds, 0, 0, 0);
		
		if (FD_ISSET(read_fd, &readfds)) {
			if (read(read_fd, message, MSG_MAX_LEN) != -1) {
				pack_message(&outgoing, message,
					username, chatroom);
				
				bytes = send(my_fd, &outgoing, sizeof(outgoing), 0);
			}
			else {
				sprintf(message, "user read failed\n");
				write(write_fd, message, strlen(message));
			}
		}
		else if (FD_ISSET(my_fd, &readfds)) {
			bytes = recv(my_fd, &incoming, sizeof(incoming), 0);
			unpack_message(&incoming, message, &in_room);
			
			if (bytes == -1) {
				sprintf(message, "Network read failed\n");
			}
			
			write(write_fd, message, strlen(message));
			
			//client is no longer in a chatroom or !join failed
			if (in_room == 0) memset(chatroom, 0, sizeof(chatroom));
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
	char message[SERVER_MAX_LEN*2];
	
	struct addrinfo hint, *data;
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	
	char ip[MSG_MAX_LEN];
	char port[MSG_MAX_LEN];
	int my_fd, status;
	
	//initial connection loop
	do {
		do {
			memset(ip, 0, sizeof(ip));
			memset(port, 0, sizeof(port));
			
			sprintf(message, "Please enter an ip address\n");
			write(write_fd, message, strlen(message));
			read(read_fd, ip, sizeof(ip)-1);
			ip[63] = 0;
			if (strchr(ip, '\n') != 0) *strchr(ip, '\n') = 0;

			sprintf(message, "Now enter the port\n");
			write(write_fd, message, strlen(message));
			read(read_fd, port, sizeof(port)-1);
			port[15] = 0;
			if (strchr(port, '\n') != 0) *strchr(port, '\n') = 0;
			
			sprintf(message, "Checking for validity: %s:%s\n", ip, port);
			write(write_fd, message, strlen(message));
		}
		while(!valid_connection(write_fd, hint, &data, ip, port));
		
		//debugging, print all returned addresses
		print_addr_list(write_fd, data);
		
		//attempt to connect with the ip/port
		my_fd = socket(data->ai_family, data->ai_socktype, data->ai_protocol);
		sprintf(message, "Created socket with fd: %d\n", my_fd);
		write(write_fd, message, strlen(message));
		
		if ( (status = connect(my_fd, data->ai_addr, data->ai_addrlen)) == -1) {
			sprintf(message, "Connection failed\n");
			write(write_fd, message, strlen(message));
			//better handling
		}
		else {
			sprintf(message, "Connection success\n");
			write(write_fd, message, strlen(message));
		}
	}
	while ( status == -1);
	
	//username creation loop
	char username[USER_MAX_LEN+1];
	
	sprintf(message, "Please enter a username (max 32 characters)\n");
	write(write_fd, message, strlen(message));
	
	read(read_fd, username, USER_MAX_LEN);
	username[USER_MAX_LEN] = 0;
	
	//consume extra characters
	int old_flags = fcntl(read_fd, F_GETFL, 0);
	fcntl(read_fd, F_SETFL, old_flags | O_NONBLOCK);
	
	read(read_fd, message, sizeof(message));
	
	fcntl(read_fd, F_SETFL, old_flags);
	
	sprintf(message, "Username entered: %s\n", username);
	write(write_fd, message, strlen(message));
	
	char *newline;
	if ( (newline = strchr(username, '\n')) != 0) *newline = 0;
	
	/*
		SEND USERNAME TO SERVER
	*/
	
	char chatroom[USER_MAX_LEN];
	memset(chatroom, 0, sizeof(chatroom));
	strncpy(chatroom, "room boi", strlen("room boi"));
	
	message_loop(read_fd, write_fd, my_fd, message, username, chatroom);
	
}

