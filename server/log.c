#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/protocol.h"

void logging(char *log, struct client_message *message) {
    sprintf(log, "[%s] %s\n", message->username, message->message);
}

void write_log(struct client_message *message) {
    char log[MSG_MAX_LEN + USER_MAX_LEN + 5];
    int fd = open(message->chatroom, O_CREAT | O_APPEND | O_WRONLY, 0600);
    logging(log, message);
    write(fd, log, strlen(log));
    close(fd);
}

int main() {
    struct client_message message;
    memset(&message, 0, sizeof(message));
    message.message_type = MT_COMMAND;
    message.chatroom = "test";
    message.username = "abc";
    message.message = "This is a message!";

    write_log(&message);
}
