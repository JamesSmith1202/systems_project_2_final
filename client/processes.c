#include<cdk/cdk.h>

#include"../include/protocol.h"
#include"processes.h"

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
	
	while( 0/*(c = getch())*/ /*== some quit key*/ ) {
		/*
		c = getch();
		
		if (c == '\n') {
			str_buf[index++] = '\n';
			str_buf[index] = 0;
			index = 0;
			write(input[1], str_buf,
				strlen(str_buf)*sizeof(char));
		}
		//else if (c == KEY_BACKSPACE) {
		else if (c == 127 || c == ) {
			if (index > 0) str_buf[--index] = 0;
			write(input[1], &c, sizeof(char));
		}
		else if (index < max_x-2) {
			str_buf[index++] = c;
			write(input[1], &c, sizeof(char));
		}
		*/
		
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
	
}

