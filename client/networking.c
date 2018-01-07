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

