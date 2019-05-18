#ifndef __R_PARSING_H__
#define __R_PARSING_H__
#include "main.h"
#include "token.h"

//#define DEBUG_REQUEST_PARSING_INIT_C
//#define DEBUG_REQUEST_PARSING_FINAL_C
//#define DEBUG_REQUEST_PARSING_LIST_C
//#define DEBUG_REQUEST_PARSING_C

// --------- enum ----------

/* request 파싱 순서 */
typedef enum requestParsingOrder {
    REQUEST_LINE = 0,
    REQUEST_HEADER,
    REQUEST_PARSING_END
}requestParsingOrder;

/* request-line 파싱 순서 */
typedef enum lineParsingOrder {
	METHOD = 0,
	URI,
	HTTP_VERSION,
    LINE_PARSING_END
}lineParsingOrder;

/* request-line (uri) 파싱 순서 */
typedef enum uriParsingOrder {
    PATH = 0,
    PATH_QUERY,
    PATH_FRAGMENT,
    URI_PARSING_END
}uriParsingOrder;

/* request-line (uri _ query) 파싱 순서 */
typedef enum queryParsingOrder {
    QUERY_ELEMENT = 0,
    QUERY_PARSING_END
}queryParsingOrder;

// --------- struct ----------

/* key / value 값을 저장 하는 구조체 */
typedef struct nameValueSet{
	token name;
	token value;
}nameValueSet;

/* list node */
typedef struct r_node {
    nameValueSet data;
    struct r_node *next;
}r_node;

/* list */
typedef struct r_list {
	r_node *head;
    r_node *tail;
	unsigned int listCount;
}r_list;

/* request-line (uri) 파싱 결과를 저장하는 구조체 */
typedef struct requestUri {
	token absolutePath;
    r_list query;
    token fragment;
}requestUri;

/* request-line 파싱 결과를 저장하는 구조체 */
typedef struct requestLine {
    token method;
    requestUri uri;
    token version;
}requestLine;

/* request 파싱 결과를 저장하는 구조체 */
typedef struct reqeust {
    requestLine line;
    r_list header;
    token garbage;
}request;


// --------- function ----------

/* linkedList.c */
void initRequestLinkedList(r_list *target);
void deleteRequestLinkedList(r_list *target);
int pushBackRequestNode(r_list *list, nameValueSet *data);
void viewRequestLinkedList(const r_list *list);

void initNameValueSet(nameValueSet *target);
void deleteNameValueSet(nameValueSet *target);

/* init.c */
void initRequestUri(requestUri *target); 
void initRequestLine(requestLine *target);
void initRequest(request *target); 

/* finalize.c */
void deleteUri(requestUri *target);
void deleteLine(requestLine *target);
void deleteRequest(request *target); 

/* requestParsing.c */
int requestParsing(const token *tokens, request *request); 
int lineParsing(char *start, char *end, requestLine *requsetLine);
int uriParsing(char *start, char *end, requestUri *requestUri); 
int queryParsing(char *start, char *end, r_list *query); 
int elementParsing(char *start, char *end, const char separate, r_list *list);
char *seekQueryFragmentToken(char *start, char *pos, uriParsingOrder *order, unsigned int *separateTokenSize);
char *seekFragmentToken(char *start, char *pos, uriParsingOrder *order, unsigned int *separateTokenSize);
void viewRequest(const request *request); 

#endif
