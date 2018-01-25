#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log.h"
#include "../include/protocol.h"

void get_timestamp(char* buffer, size_t bufferLen) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, bufferLen, "%H:%M:%S", timeinfo);
}

void get_date(char* buffer, size_t bufferLen) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, bufferLen, LOG_DATE_FORMAT, timeinfo);
}

void logging(char *log, struct client_message *message) {
    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    sprintf(log, LOG_FORMAT, timestamp, message->username, message->message);
}

void write_log(struct client_message *message) {
    char log[MSG_MAX_LEN + USER_MAX_LEN + 5 + 32];
    mkdir(message->chatroom, 0700);

    char path[64];
    char date[16];
    get_date(date, sizeof(date));
    sprintf(path, "%s/%s/%s", LOG_BASEDIR, message->chatroom, date);

    int fd = open(path, O_CREAT | O_APPEND | O_WRONLY, 0600);
    logging(log, message);
    write(fd, log, strlen(log));
    close(fd);
}

void read_log(char *buffer, char *chatroom, char *date) {
    char path[64];
    sprintf(path, "%s/%s/%s", LOG_BASEDIR, chatroom, date);

    int fd = open(path, O_RDONLY, 0600);
    read(fd, buffer, sizeof(buffer));
}

/*
int main() {
    struct client_message message;
    memset(&message, 0, sizeof(message));
    message.message_type = MT_COMMAND;
    strncpy(message.chatroom, "test", sizeof(message.chatroom));
    strncpy(message.username, "abc", sizeof(message.username));
    strncpy(message.message, "This is a message!", sizeof(message.message));

    write_log(&message);

    char buffer[10000];
    read_log(buffer, "test", "20180119");
    printf("%s\n", buffer);
}
*/
