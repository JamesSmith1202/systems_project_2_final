#include<cdk/cdk.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/sem.h>

#include<unistd.h>
#include<fcntl.h>

#define SHM_KEY 10

/*
void child_print(WINDOW *w, int max_x, int max_y) {
	char s[128];
	while(1) {
		move(max_y-6, 1);
		wgetstr(w, s);
		wprintw(w, s);
		wrefresh(w);
	}
}

void parent_print(WINDOW *w, int max_x, int max_y) {
	CDKSCREEN *screen;
	CDKSWINDOW *scroll;
	
	screen = initCDKScreen(w);
	
	scroll = newCDKSwindow(
		screen,
		1, 1,
		max_y-7, max_x-2,
		"log",
		10,
		1,
		0
	);
	
	
	
	int x = 0;
	char temp[20];
	while(1) {
		sprintf(temp, "parent: %d", x++);
		//wprintw(w, "parent\n");
		addCDKSwindow(scroll, temp, BOTTOM);
		//move(max_y-6, 1);
		//wrefresh(w);
		//refreshCDKScreen(screen);
		//move(max_y-6, 1);
		drawCDKSwindow(scroll, 1);
		
		//refreshCDKScreen(screen);
		//wrefresh(w);
		move(max_y-6, 1);
		wrefresh(w);
		move(max_y-6, 1);
		sleep(1);
	}
}
*/

int main() {
	WINDOW *child, *parent;
	int max_x, max_y;
	
	initscr();
	cbreak();
	echo();
	getmaxyx(stdscr, max_y, max_x);
	
	//height, width, y, x
	child = newwin(5, max_x, max_y-5, 0);
	parent = newwin(max_y-5, max_x, 0, 0);
	
	box(child, 0, 0);
	//box(parent, 0, 0);
	
	
	refresh();
	wrefresh(child);
	wrefresh(parent);

	CDKSCREEN *screen;
	CDKSCREEN *child_screen;
	CDKSWINDOW *scroll;
	
	screen = initCDKScreen(parent);
	child_screen = initCDKScreen(child);
	
	scroll = newCDKSwindow(
		screen,
		1, 1,
		max_y-7, max_x,
		"log",
		10,
		1,
		0
	);
	
	//0 is read, 1 is write
	int input[2];
	int log[2];
	pipe(input);
	pipe(log);
	
	int flags = fcntl(input[0], F_GETFL, 0);
	fcntl(input[0], F_SETFL, flags | O_NONBLOCK);
	
	flags = fcntl(log[0], F_GETFL, 0);
	fcntl(log[0], F_SETFL, flags | O_NONBLOCK);

	
	int is_parent = fork();
	
	if (is_parent) {
		is_parent = fork();
		
		//main graphics process
		if (is_parent) {
		
			char s[128];
			memset(s, 0, sizeof(s));
			int read_status;
			int cur_x, cur_y;
			move(max_y-4, 1);
			getyx(stdscr, cur_y, cur_x);
			while(1) {
				memset(s, 0, sizeof(s));
				read_status = read(input[0], s, sizeof(s));
				if (read_status != -1) {
					if (strlen(s) > 1) {
						mvwprintw(child, max_y-4, 1, "");
						addCDKSwindow(scroll, s, BOTTOM);
						refreshCDKScreen(screen);
						while (cur_x > 1) {
							move(cur_y, --cur_x);
							addch(' ');
							move(cur_y, cur_x);
						}
						refreshCDKScreen(screen);
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
				read_status = read(log[0], s, sizeof(s));
				if (read_status != -1) {
					addCDKSwindow(scroll, s, BOTTOM);
					refreshCDKScreen(screen);
				}
				move(cur_y, cur_x);
				refresh();
				move(cur_y, cur_x);
				//sleep(1);
			}
		}
		//log updater process
		else {
			char temp[40];
			int x = 0;
			while(1) {
				sprintf(temp, "parent: %d", x++);
				write(log[1], temp, sizeof(temp));
				sleep(1);
			}
		}
	}
	//user input process
	else {
		//child_print(child, max_x, max_y);
		int c;
		char str_buf[128];
		//echo();
		int index = 0;
		while(1) {
			//new solution, use a bunch of getch() calls
			//and return those so main loop can move cursor
			
			//move(max_y-7, 1);
			c = getch();
			//wprintw(child, s);
			//addCDKSwindow(scroll, s, BOTTOM);
			//refreshCDKScreen(screen);
			
			//wrefresh(child);
			//refreshCDKScreen(child_screen);
			//move(max_y-7, 1);
			
			if (c == '\n') {
				str_buf[index++] = '\n';
				str_buf[index] = 0;
				index = 0;
				write(input[1], str_buf,
					strlen(str_buf)*sizeof(char));
			}
			//else if (c == KEY_BACKSPACE) {
			else if (c == 127) {
				if (index > 0) str_buf[--index] = 0;
				write(input[1], &c, sizeof(char));
			}
			else {
				str_buf[index++] = c;
				write(input[1], &c, sizeof(char));
			}
		}
		
	}
	
	getch();
	endwin();
	
	return 0;
}

