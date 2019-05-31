#ifndef __PARSING_PACKET_H__
#define __PARSING_PACKET_H__
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
}requestParsingOrder;

/* request-line 파싱 순서 */
typedef enum lineParsingOrder {
	METHOD = 0,
	URI,
	HTTP_VERSION,
}lineParsingOrder;

/* request-line (uri) 파싱 순서 */
typedef enum uriParsingOrder {
    PATH = 0,
    PATH_QUERY,
    PATH_FRAGMENT,
}uriParsingOrder;

/* header element kinds */
typedef enum headerKind {
    ACCEPT = 0,
    ACCEPT_CHARSET,
    ACCEPT_ENCODING,
    ACCEPT_LANGUAGE,
    AUTHORIZATION,
    EXPECT,
    FROM,
    HOST,
    IF_MATCH,
    IF_MODIFIED_SINCE,
    IF_NONE_MATCH,
    IF_RANGE,
    IF_UNMODIFIED_SINCE,
    MAX_FROWARDS,
    PROXY_AUTHORIZATION,
    RANGE,
    REFERER,
    TE,
    USER_AGENT,
    MAX_HEADER_ELEMENT_COUNT
}headerKind;

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

/* request-Header 파싱 결과를 저장하는 구조체 */
typedef struct requestHeader {  
  r_list header;
  token *headerElements[MAX_HEADER_ELEMENT_COUNT];
}requestHeader;

/* request 파싱 결과를 저장하는 구조체 */
typedef struct reqeuest {
    requestLine line;
    requestHeader header;
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
void initRequestHeader(requestHeader *target);
void initRequest(request *target); 

/* finalize.c */
void deleteUri(requestUri *target);
void deleteLine(requestLine *target);
void deleteHeader(requestHeader *target);
void deleteRequest(request *target); 

/* requestParsing.c */
int requestParsing(const token *tokens, request *request); 
int headerEleParsing(char *start, char *end, requestHeader * requestHeader);
int lineParsing(char *start, char *end, requestLine *requsetLine);
int uriParsing(char *start, char *end, requestUri *requestUri); 
int queryParsing(char *start, char *end, r_list *query); 
int queryEleParsing(char *start, char *end, r_list *list);
char *seekQueryFragmentToken(char *start, char *pos, uriParsingOrder *order);
char *seekFragmentToken(char *start, char *pos, uriParsingOrder *order);
void viewRequest(const request *request); 

#endif
