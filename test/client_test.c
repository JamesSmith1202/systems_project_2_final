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
        char chatroom[CHATROOM_MAX_LEN+1];//message destination
	char username[USER_MAX_LEN+1]; //display name
        char message[MSG_MAX_LEN+1];
};

void pack_message(struct client_message *outgoing, char *msg,
		char *username, char *chatroom, short *disconnect) {
	if (strlen(msg) < 1) return;
	
	//printf("%s\n", msg);
	//printf("%lu\n", strlen(msg));
	
	if (msg[0] == '!') {
		outgoing->message_type = MT_COMMAND;
		
		char *token, *temp;
		char copy[MSG_MAX_LEN];
		
		//strip the '!'
		memmove(copy, msg+1, strlen(msg));
		temp = copy;
		
		token = strsep(&temp, " ");
		if (token == 0 || strlen(token) < 1) {
			
			outgoing->message_type = MT_ERR;
			
			return;
		}
		
		if (!strcmp(copy, "msg")) {
			//get the target chatroom name
			token = strsep(&temp, " ");
			if (token == 0) {
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
			chatroom[CHATROOM_MAX_LEN] = 0;
			//printf("%s\n", chatroom);
			//printf("%lu\n", strlen(chatroom));
			strncpy(outgoing->chatroom, chatroom, strlen(chatroom)+1);
		}
		else if (!strcmp(copy, "disconnect")) {
			*disconnect = 1;
			return;
		}
	}
	else {
		outgoing->message_type = MT_MESSAGE;
		strncpy(outgoing->chatroom, chatroom, strlen(chatroom));
	}
	
	//printf("%s\n", username);
	//printf("%lu\n", strlen(username));
	
	//printf("%s\n", msg);
	//printf("%lu\n", strlen(msg));

	strncpy(outgoing->username, username, strlen(username)+1);
	strncpy(outgoing->message, msg, strlen(msg)+1);
}

int main(void) {
    struct client_message out;
    char user[USER_MAX_LEN];
    char chat[CHATROOM_MAX_LEN+1];
    
    char msg[] = "!join 123456789012345678901234567890EE";
    
	short disconnect = 0;
	
    strncpy(user, "Bob", 4);
    strncpy(chat, "room", 5);
    
    pack_message(&out, msg, user, chat, &disconnect);
    
    printf("%d: %s, %s, %s\n", out.message_type, out.chatroom, out.username, out.message);
    
    printf("%d: %lu, %lu, %lu\n", out.message_type, strlen(out.chatroom), strlen(out.username), strlen(out.message));
    
	return 0;
}


