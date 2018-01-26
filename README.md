# systems_project_2_final
### Stanley Lin, James Smith, Ryan Siu
### Period 10

### Description
A non-blocking TCP socket chat server that can sustain many chat rooms and users.

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

##### Notes on the client
* The client can support the BACKSPACE key, but no other special function keys (like DELETE or any arrow keys).
* TAB is supported, but the input display may become messed up.
* It is recommended to use a fullscreen terminal window for the client. The client doesn't support resizing windows.

##### Notes on the server 

