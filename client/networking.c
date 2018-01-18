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
	if (strlen(msg) < 1) return 0;
	
	if (msg[0] == '!') {
		strncpy(outgoing->message_type, MT_COMMAND,);
		//outgoing->message_type = MT_COMMAND;
		
		char token[64];
		char copy[MSG_MAX_LEN];
		
		//strip the '!'
		memmove(copy, msg+1, MSG_MAX_LEN - 1);
		token = strsep(&copy, " ");
		
		if (!strcmp(copy, "msg")) {
			//get the target chatroom name
			token = strsep(&copy, " ");
			
			strncpy(outgoing->chatroom, token, strlen(token)+1);
		}
	}
	else {
		outgoing->message_type = MT_MESSAGE;
		outgoing->chatroom = chatroom;
		strncpy(outgoing->message_type, ,);
	}
	
	strncpy(outgoing->username, username, strlen(username));
	strncpy(outgoing->message, msg, strlen(msg));
	strncpy(outgoing->, ,);
}

void unpack_message(struct server_message *incoming, char *msg) {
	
}



