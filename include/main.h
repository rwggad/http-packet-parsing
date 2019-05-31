#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SUCCESS             0
#define FAIL                -1
#define MATCHING            0
#define NOT_MATCHING        -2

#define IS_SAME             0 
#define IS_DIFF             -1

#define EXIST               0
#define NO_EXIST            -1

#define SP                  ' '
#define CR                  '\r'
#define LF                  '\n'
#define QUERY               '?'
#define QUERY_AND           '&'
#define FRAGMENT            '#'
#define ASSIGN              '='
#define COLON               ':'
#define COMMA               ','
#define FULL_STOP           '.'
#define BACK_SLASH          '/'
#define UNDER_BAR           '_'

#define PARAM_EXP_CHECK_NO_RETURN(target) \
    if (target == NULL) { \
        printf("[Exception] param is null pointer. (param : " #target ")\n"); \
        return; \
    }

#define PARAM_EXP_CHECK(target, returnType) \
    if(target == NULL) { \
        printf("[Exception] param is null pointer. (param :" #target ")\n"); \
        return returnType; \
    } 

#define RANGE_EXP_CHECK(cmp1, cmp2, returnType) \
    if(cmp1 > cmp2) { \
        printf("[Exception] start pointer must be smaller than end pointer.("#cmp1">"#cmp2") \n"); \
        return returnType; \
    } 


//#define DEBUG_FILE_READ_C

#endif
