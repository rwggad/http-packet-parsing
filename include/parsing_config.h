#ifndef __PARSING_CONFIG_H__
#define __PARSING_CONFIG_H__
#include "main.h"
#include "token.h"
#include "commonApi.h"

//#define DEBUG_CONFIG_PARSING_INIT_C     
//#define DEBUG_CONFIG_PARSING_FINAL_C    
//#define DEBUG_CONFIG_PARSING_LIST_C     
//#define DEBUG_CONFIG_PARSING_C          

#define MIN_PORT_NUMBER_RANGE   0
#define MAX_PORT_NUMBER_RANGE   65535

#define MAX_SERVER_SIZE         2048

// --------- enum ----------
typedef enum elementParsingOrder {
    ELEMENT_MATCH_TYPE = 0,
    ELEMENT_MATCH_METHOD,
    ELEMENT_MATCHING_STRING,
    ELEMENT_SERVER_LIST,
}elementParsingOrder;

typedef enum configMatchType {
    TYPE_PATH = 0,
    TYPE_HOST
}matchType;

typedef enum configMatchMethod {
    METHOD_ANY = 0,
    METHOD_START,
    METHOD_END
}matchMethod;

// --------- struct ----------

typedef struct ipSet {
    struct sockaddr_in addr;
}ipSet;

/* list node dataSet */
typedef struct configDataSet {
    matchType type;
    matchMethod method;
    token matchingString;
    unsigned int runTimeIndex;
    unsigned int serverListSize;
    ipSet serverList[MAX_SERVER_SIZE];
}configDataSet;

/* list node */
typedef struct configNode {
    configDataSet data;
    struct configNode *next;
}configNode;

/* list */
typedef struct configList {
	configNode *head;
    configNode *tail;
	unsigned int listCount;
}configList;

/* confing data list */
typedef struct configTable {
    configList config;
}configTable;


// --------- function ----------

/* linkedList.c */
int pushBackConfigNode(configList *list, configDataSet *data);
void viewConfigList(const configList *list);
void initConfigList(configList *target);
void deleteConfigList(configList *target);

void initConfigDataSet(configDataSet *target);
void deleteConfigDataSet(configDataSet *target);

/* init.c */
void initConfig(configTable *configTable);

/* finalize.c */
void deleteConfig(configTable *configTable);

/* parsing.c */
int configParsing(const token *tokens, configTable *configList);
int configElementParsing(char *start, char *end, configDataSet *dataSet);
int configServerParsing(char *start, char *end, configDataSet *dataSet);
int configIpPortParsing(char *start, char *end, ipSet *server);

#endif
