# systems_project_2_final
### Stanley Lin, James Smith, Ryan Siu
### Period 10

### Description
A multiplexing TCP socket chat server that can sustain many chat rooms and users and its corresponding client.

### Dependencies
You will need to install the following libraries through apt-get:

* `libncurses5-dev`
* `libncursesw5-dev`
* `ncurses-term`
* `libcdk5-dev`

Or just copy-paste this: `sudo apt-get install libncurses5-dev libncursesw5-dev ncurses-term libcdk5-dev`

### Usage
* In the root of the directory, type `make` into a command line.
* Then, type `./client_run` to run the client program.
* Open up another terminal window and type `./server_run` to run the server.
* The server will print a message declaring its IP and server like so 
```
Starting server setup...
Server is listening on 0.0.0.0:3490
```
* The client can then enter the information and connect.
* The client will be welcomed and can use `!help` to avail themselves to the available commands. They can be seen below
```
commands the user can use:
	!list:			list chatrooms
	!join <room>:		join a chatroom or create a new room if <room> is not found
	!leave:			leave the current room
	!msg <room> <message>:	message the indicated room
	!history:		see a log of the messages in the current room
	!help:			lists all available commands
	!disconnect:	        disconnects the client from the server
```

##### Notes on the client
* The client can support the BACKSPACE key, but no other special function keys (like DELETE or any arrow keys).
* TAB is supported, but the input display may become messed up.
* It is recommended to use a fullscreen terminal window for the client. The client doesn't support resizing windows.
* The open endedness of the client ip/port selection allows for the possibility of connecting to different servers(given that you run the servers on different IPs or ports, but they are hardcoded currently)
* Fixed size buffers restrict size of data transfer

##### Notes on the server 
* The server does not support unique usernames
* Fixed size buffers restrict size of data transfer
