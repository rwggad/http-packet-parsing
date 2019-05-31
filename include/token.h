#ifndef __TOKEN_H__
#define __TOKEN_H__
#include "main.h"

//#define DEBUG_TOKEN_C
#define IS_SAME     0 
#define IS_DIFF     -1

// ----- strcut -----

/* token */
typedef struct token {
    char *value;
    unsigned int size;
}token;

// ----- function ----- 

/* token.c */
void initToken(token *token);
void deleteTokenValue(token *token); 
int saveTokenValue(token *token, char *start, char *end);
void viewToken(const token *token);

#endif
