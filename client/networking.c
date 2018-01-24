#include"../include/protocol.h"
#include"networking.h"

void print_addr_list(int write_fd, struct addrinfo *data) {
	char message[MSG_MAX_LEN];
	char addr_temp[128];

	while (data != 0) {
		struct sockaddr_in *ip = (struct sockaddr_in *)data->ai_addr;
		
		//pretty printing
		inet_ntop(AF_INET, &(ip->sin_addr), addr_temp, data->ai_addrlen);
		
		sprintf(message, "IP result: %s\n", addr_temp);
		write(write_fd, message, strlen(message));
		
		data = data->ai_next;
	}
}

int valid_connection(int write_fd, struct addrinfo hint, struct addrinfo **data,
		char *ip, char *port) {
	int res;
	char message[MSG_MAX_LEN];
	
	if ( (res = getaddrinfo(ip, port, &hint, data)) != 0) {
		sprintf(message, "%s\n", gai_strerror(res));
		write(write_fd, message, strlen(message));
		
		return 0;
	}
	
	return 1;
}

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
	}
	else {
		outgoing->message_type = MT_MESSAGE;
		strncpy(outgoing->chatroom, chatroom, strlen(chatroom));
	}
	
	strncpy(outgoing->username, username, strlen(username));
	strncpy(outgoing->message, msg, strlen(msg)+1);
}

void unpack_message(struct server_message *incoming, char *msg) {
	char server[] = "[SERVER]";
	char error[] = "[ERROR]";
	char delim[] = ": ";
	
	switch(incoming->message_type) {
		case MT_COMMAND:
			strncat(msg, server, strlen(server));
		break;
		
		case MT_MESSAGE:
			strncat(msg, incoming->username, strlen(incoming->username));
		break;
		
		case MT_ERR:
			strncat(msg, error, strlen(error));
		break;
	}
	
	strncat(msg, delim, strlen(delim));
	strncat(msg, incoming->message, strlen(incoming->message));
}



