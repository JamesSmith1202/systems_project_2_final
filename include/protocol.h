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
#define MAX_USERS	16

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
        char chatroom[CHATROOM_MAX_LEN];//message destination
	char username[USER_MAX_LEN]; //display name
        char message[MSG_MAX_LEN];
};

//a chat room on the server. We will have a list of these
struct chat_room{
	char name[CHATROOM_MAX_LEN];//server name
	int users[MAX_USERS];//list of client socket fd’s
        int num_users; //number of users currently connected
};

struct server_message{//received by client to display to user
	unsigned short message_type;//message vs return value of command
        char username[USER_MAX_LEN]; //display name
	char message[SERVER_MAX_LEN];
};

#endif
