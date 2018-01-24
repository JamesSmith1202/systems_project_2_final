#ifndef PROTOCOL_H
#define PROTOCOL_H

//message type
#define MT_COMMAND	0
#define MT_MESSAGE	1
#define MT_ERR		65535

//max length of a user sent message
#define MSG_MAX_LEN	256

//max username length
#define USER_MAX_LEN	32

//max length for a server sent mesage
#define SERVER_MAX_LEN	8192

//max length for a chatroom's name
#define CHATROOM_MAX_LEN 32

//max number of users in one chat room
#define MAX_USERS 16

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
reason why the strings are +1 length is for the null terminator
*/

struct client_message{ //the message the client sends to server
    unsigned short message_type;//command vs message
    char chatroom[CHATROOM_MAX_LEN+1];//message destination
    char username[USER_MAX_LEN+1]; //display name
    char message[MSG_MAX_LEN+1];
};

//a chat room on the server. We will have a list of these
struct chat_room{
    char name[CHATROOM_MAX_LEN+1];//server name
    int users[MAX_USERS];//list of client socket fd’s
    int num_users; //number of users currently connected
};

/*
Note on the in_chatroom field:

the client will not be in a chatroom initially

client will request to join one through the !join command
	if the in_chatroom field is == 0 in the response, the server did
	not add the client to the chat room
	
	if it is 1, then client has successfully joined the room

everytime the server sends a message to a client, the server should always
	specify whether or not the client is still in the room.

the client will assume it is no longer in a chat room if
	in_chatroom == 0
	
	at this point, the client should attempt to join a chatroom again
	with !join

if the client uses the !leave command, it is expected that the server will
	respond with in_chatroom == 0

*/
struct server_message{//received by client to display to user
    unsigned short message_type;//message vs return value of command
    char username[USER_MAX_LEN+1]; //display name
    char message[SERVER_MAX_LEN+1];
    char in_chatroom;//should be 0 if not currently in chatroom, 1 if they are
};

#endif
