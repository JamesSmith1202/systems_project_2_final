//message type
#define MT_COMMAND 0
#define MT_MESSAGE 1


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
