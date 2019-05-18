#ifndef __FILE_READ_H__
#define __FILE_READ_H__

#include "main.h"
#include "token.h"

#define GET_FILE_STAT_FAIL -1
#define MAX_FILE_PATH_LENGTH 2048

int fileRead(token *token, const char *filePath);
void clearInputBuffer(void);

#endif
