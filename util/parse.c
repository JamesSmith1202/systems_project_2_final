#include <stdlib.h>
#include <ctype.h>

#include "../include/parse.h"

char* strip(char *line) {
	char *result = (char*)malloc(sizeof(line));
	//strip the front
	while ( *line && isspace( *line ) ) line++;

	//strip the back
	char *back = strchr(line, '\0')-1;
	while ( back > line && isspace( *back ) ) back--;
	if (back < line) return 0;
	*(back+1) = 0;

	//create the new string
	result = line;
	int index = 0;
	while (*line) *(result + (index++)) = *(line++);

	return result;
}

