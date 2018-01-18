#ifndef PROTOCOL_H
#define PROTOCOL_H

//message type
#define MT_COMMAND 0
#define MT_MESSAGE 1

//max length of a user sent message
#define MSG_MAX_LEN	256

//max username length
#define USER_MAX_LEN	32

//max length for a server sent mesage
#define SERVER_MAX_LEN	8192

//max length for a chatroom's name
#define CHATROOM_MAX_LEN 32

//max number of users in one chat room
#define MAX_USERS


/*
	commands the user can use (ideas):
	!list:			list chatrooms
	!join <room>:		join a chatroom
	!leave:			leave the current room
	!msg <room> <message>:	message the indicated room
	!history:		see a log of the messages in the current room
	!help:			lists all available commands
*/



/*
	commands the user can use (ideas):
	!list:			list chatrooms
	!join <room>:		join a chatroom
	!leave:			leave the current room
	!msg <room> <message>:	message the indicated room
	!history:		see a log of the messages in the current room
	!help:			lists all available commands
*/

struct client_message{ //the message the client sends to server
	unsigned short message_type;//command vs message
        char[CHATROOM_MAX_LEN] chatroom;//message destination
	char[USER_MAX_LEN] username; //display name
        char[MSG_MAX_LEN] message;
};

//a chat room on the server. We will have a list of these
struct chat_room{
	char[CHATROOM_MAX_LEN] name;//server name
	int[MAX_USERS] users;//list of client socket fd’s
        int num_users; //number of users currently connected
};

struct server_message{//received by client to display to user
	unsigned short message_type;//message vs return value of command
        char[USER_MAX_LEN] username; //display name
	char[SERVER_MAX_LEN] message;
};

#endif
