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
	char *chatroom;//message destination
	char *username; //display name
	char *message;
};

//a chat room on the server. We will have a list of these
struct chat_room{
	char *name;//server name
	int *users;//list of client socket fd’s
};

struct server_message{//received by client to display to user
	unsigned short message_type;//message vs return value of command
	char *username; //display name
	char *message;
};

#endif

