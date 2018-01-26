#ifndef LOG_H
#define LOG_H

#include <stdlib.h>
#include "../include/protocol.h"

#define LOG_DATE_FORMAT "%G%m%d"
#define LOG_FORMAT "[%s] %s: %s\n"
#define LOG_BASEDIR "data"

void get_timestamp(char* buffer, size_t bufferLen);
void get_date(char* buffer, size_t bufferLen);
void logging(char *log, struct client_message *message);
void write_log(struct client_message *message);
int read_log(char *buffer, size_t buffer_size, char *chatroom, char *date);

#endif
