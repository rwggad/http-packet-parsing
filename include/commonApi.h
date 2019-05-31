#ifndef __COMMON_API_H__
#define __COMMON_API_H__
#include "main.h"

#define MIN_PORT_NUMBER_RANGE   0
#define MAX_PORT_NUMBER_RANGE   65535

int str_inet_pton(char *string, struct sockaddr_in *addr);
int mem_inet_pton(char *start, char *end, struct sockaddr_in *addr);
int mem_str_i_cmp(const char *start, const char *end, const char *string);
int mem_atoi(const char *start, const char *end);
int port_strtoul(const char *strPort, unsigned short *port);

#endif
