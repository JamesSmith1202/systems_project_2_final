#include"networking.h"

void print_addr_list(int write_fd, struct addrinfo *data) {
	char message[256];
	char addr_temp[128];
	while (data != 0) {
		struct sockaddr_in *ip = (struct sockaddr_in *)data->ai_addr;
		inet_ntop(AF_INET, &(ip->sin_addr), addr_temp, data->ai_addrlen);
		
		sprintf(message, "IP result: %s\n", addr_temp);
		write(write_fd, message, strlen(message));
		
		data = data->ai_next;
	}
}

