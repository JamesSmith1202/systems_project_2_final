#include"networking.h"

void print_addr_list(int write_fd, struct addrinfo *data) {
	char message[MSG_MAX_LEN];
	char addr_temp[128];
	//sprintf(message, "printing list\n");
	write(write_fd, message, strlen(message));

	while (data != 0) {
		//sprintf(message, "inside\n");
		write(write_fd, message, strlen(message));

		struct sockaddr_in *ip = (struct sockaddr_in *)data->ai_addr;
		inet_ntop(AF_INET, &(ip->sin_addr), addr_temp, data->ai_addrlen);
		
		sprintf(message, "IP result: %s\n", addr_temp);
		write(write_fd, message, strlen(message));
		
		data = data->ai_next;
	}
	//sprintf(message, "end list\n");
	write(write_fd, message, strlen(message));

}

int valid_connection(int write_fd, struct addrinfo hint, struct addrinfo **data,
		char *ip, char *port) {
	int res;
	char message[MSG_MAX_LEN];
	sprintf(message, "valid ip test: %s\n", ip);
	write(write_fd, message, strlen(message));

	if ( (res = getaddrinfo(ip, port, &hint, data)) != 0) {
		sprintf(message, "%s\n", gai_strerror(res));
		write(write_fd, message, strlen(message));
		
		return 0;
	}
	
	write(write_fd, message, strlen(message));
	
	return 1;
}

