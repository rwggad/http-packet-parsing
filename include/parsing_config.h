#ifndef __C_PARSING_H__
#define __C_PARSING_H__
#include "main.h"
#include "token.h"

//#define DEBUG_CONFIG_PARSING_INIT_C     
//#define DEBUG_CONFIG_PARSING_FINAL_C    
//#define DEBUG_CONFIG_PARSING_LIST_C     
#define DEBUG_CONFIG_PARSING_C          

#define MAX_SERVER_SIZE                 2048

// --------- enum ----------
typedef enum elementParsingOrder {
    ELEMENT_MATCH_TYPE = 0,
    ELEMENT_MATCH_METHOD,
    ELEMENT_MATCHING_STRING,
    ELEMENT_SERVER_LIST,
}elementParsingOrder;

// --------- struct ----------

/* list node dataSet */
typedef struct configDataSet {
    token matchType;
    token matchMethod;
    token matchingString;
    unsigned int runTimeIndex;
    unsigned int serverListSize;
    token serverList[MAX_SERVER_SIZE];
}configDataSet;

/* list node */
typedef struct c_node {
    configDataSet data;
    struct c_node *next;
}c_node;

/* list */
typedef struct c_list {
	c_node *head;
    c_node *tail;
	unsigned int listCount;
}c_list;

/* confing data list */
typedef struct config {
    c_list configDataList;
}config;


// --------- function ----------

/* linkedList.c */
int pushBackConfigNode(c_list *list, configDataSet *data);
void viewConfigLinkedList(const c_list *list);
void initConfigLinkedList(c_list *target);
void deleteConfigLinkedList(c_list *target);

void initConfigDataSet(configDataSet *target);
void deleteConfigDataSet(configDataSet *target);

/* init.c */
void initConfig(config *configData);

/* finalize.c */
void deleteConfig(config *configData);

/* parsing.c */
int configParsing(const token *tokens, config *configData);
int configElementParsing(char *start, char *end, configDataSet *dataSet);
int configServerParsing(char *start, char *end, configDataSet *dataSet);
#endif
