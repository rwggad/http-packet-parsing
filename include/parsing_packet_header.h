#ifndef __PARSING_PACKET_HEADER_H__
#define __PARSING_PACKET_HEADER_H__
#include "main.h"
#include "commonApi.h"

//#define DEBUG_PACKET_HEADER_PARSING_INIT_C     
//#define DEBUG_PACKET_HEADER_PARSING_FINAL_C    
//#define DEBUG_PACKET_HEADER_PARSING_LIST_C    
//#define DEBUG_PACKET_HEADER_PARSING_C

// -------- enum ---------
typedef enum protocolType {
    TCP = 6,
    UDP = 17
}protocolType;

// --------- struct ----------

/* 5 tuple */
typedef struct tuple5 {
    struct sockaddr_in src;
    struct sockaddr_in dest;
    protocolType protocol;
} tuple5;

/* session set */
typedef struct session {
    tuple5 tuple5;
    struct sockaddr_in sessionServer;
} session;

/* list node */
typedef struct sessionNode {
    session data;
    struct sessionNode *next;
} sessionNode;

/* list */
typedef struct sessionList {
	sessionNode *head;
    sessionNode *tail;
	unsigned int listCount;
} sessionList;

/* session table */
typedef struct sessionTable {
    sessionList sessionList;
} sessionTable;

// --------- function ----------

/* linkedList.c */
int pushBackSessionList(sessionList *list, session *data);
void popFrontSessionList(sessionList *list, session *popSession);
void initSessionList(sessionList *target);
void deleteSessionList(sessionList *target);

/* init.c */
void initTuple5(tuple5 *target);
void initSession(session *target);
void initSessionTable(sessionTable *target);

/* finalize.c */
void deleteSessionTable(sessionTable *target);

/* parsing.c */
int packetHeaderParsing(char *path, tuple5 *tuple5);


#endif
