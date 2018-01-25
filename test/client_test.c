#include <stdio.h>
#include<string.h>
#include<stdlib.h>


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
#define MAX_USERS	16


struct client_message{ //the message the client sends to server
	unsigned short message_type;//command vs message
        char chatroom[CHATROOM_MAX_LEN];//message destination
	char username[USER_MAX_LEN]; //display name
        char message[MSG_MAX_LEN];
};
/*
void pack_message(struct client_message *outgoing, char *msg,
		char *username, char *chatroom) {
	if (strlen(msg) < 1) return;
	
	if (msg[0] == '!') {
		outgoing->message_type = MT_COMMAND;
		printf("! found\n");
		
		char *token, *temp;
		char copy[MSG_MAX_LEN];
		
		//strip the '!'
		memmove(copy, msg+1, strlen(msg) - 1);
		temp = copy;
		printf("%s\n", copy);
		
		token = strsep(&temp, " ");
		printf("%s\n", token);
		
		if (!strcmp(copy, "msg")) {
			printf("in msg\n");
			//get the target chatroom name
			token = strsep(&temp, " ");
			printf("%s\n", token);
			
			strncpy(outgoing->chatroom, token, strlen(token)+1);
		}
	}
	else {
		outgoing->message_type = MT_MESSAGE;
		strncpy(outgoing->chatroom, chatroom, sizeof(chatroom));
	}
	
	strncpy(outgoing->username, username, strlen(username));
	strncpy(outgoing->message, msg, strlen(msg)+1);
}
*/

void pack_message(struct client_message *outgoing, char *msg,
		char *username, char *chatroom) {
	if (strlen(msg) < 1) return;
	
	if (msg[0] == '!') {
		//printf("found !\n");
		outgoing->message_type = MT_COMMAND;
		
		char *token, *temp;
		char copy[MSG_MAX_LEN];
		
		//strip the '!'
		memmove(copy, msg+1, strlen(msg) - 1);
		temp = copy;
		
		token = strsep(&temp, " ");
		//printf("%s\n", token);
		if (token == 0 || strlen(token) < 1) {
			//printf("no arg\n");
			
			outgoing->message_type = MT_ERR;
			
			return;
		}
		
		if (!strcmp(copy, "msg")) {
			//get the target chatroom name
			token = strsep(&temp, " ");
			if (token == 0) {
				//printf("no room\n");
				outgoing->message_type = MT_ERR;
				memset(&outgoing, 0, sizeof(outgoing));
				return;
			}
			
			strncpy(outgoing->chatroom, token, strlen(token)+1);
		}
		else if (!strcmp(copy, "join")) {
			token = strsep(&temp, " ");
			if (token == 0) {
				outgoing->message_type = MT_ERR;
				memset(&outgoing, 0, sizeof(outgoing));
				return;
			}
			
			memset(chatroom, 0, sizeof(chatroom));
			strncpy(chatroom, token, strlen(token));
			strncpy(outgoing->chatroom, token, strlen(token));
			
			chatroom[strlen(token)] = 0;
			outgoing->chatroom[strlen(token)] = 0;
		}
	}
	else {
		outgoing->message_type = MT_MESSAGE;
		strncpy(outgoing->chatroom, chatroom, strlen(chatroom));
	}
	
	strncpy(outgoing->username, username, strlen(username));
	strncpy(outgoing->message, msg, strlen(msg)+1);
}

int main(void) {
    struct client_message out;
    char user[USER_MAX_LEN];
    char chat[CHATROOM_MAX_LEN];
    
    char msg[] = "um";
    
    strncpy(user, "Bob", 4);
    strncpy(chat, "room", 5);
    
    pack_message(&out, msg, user, chat);
    
    printf("%d: %s, %s, %s\n", out.message_type, out.chatroom, out.username, out.message);
    
    printf("%d: %lu, %lu, %lu\n", out.message_type, strlen(out.chatroom), strlen(out.username), strlen(out.message));
    
	return 0;
}


