#include "parsing_packet.h"
/**
 * request 파싱 함수
 *
 * @param   tokens  파싱할 토큰들이 저장된 구조체
 * @param   request 파싱 후 토큰들을 저장할 구조체
 * @return  int     파싱 성공 여부 반환 (SUCCESS 1, FAIL -1)
*/
int requestParsing(const token *tokens, request *request)
{
    char *start = NULL;
    char *end = NULL;
    char *pos = NULL;
    int parsingResult = 0;
    unsigned int remainedTokenSize = 0;
    requestParsingOrder parsingOrder = 0;

    /* param exception */
    PARAM_EXP_CHECK(tokens, FAIL);
    PARAM_EXP_CHECK(request, FAIL);

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request parsing start --- \n");
#endif

    /* set pointer */
    start = (tokens->value); // set pointer
    end = ((tokens->value + tokens->size) - 1); // set pointer
    
    /* parsing */
    parsingResult = SUCCESS;
    parsingOrder = REQUEST_LINE;
    while (start <= end ) {
        remainedTokenSize = ((end - start) + 1); // start에서 검색할 바이트 범위 설정
        pos = memchr(start, LF, remainedTokenSize);
        if (parsingOrder == REQUEST_LINE) {
            if (pos == NULL) {
                printf("[Exception] no exist LF token. (request function) \n");
                parsingResult = FAIL;
                goto parsingFail;
            }
            if ((pos - start) < 2) { // CLFR 유효범위, 값있는지 체크
                printf("[Exception] requestLine is essentail element. (request function)\n");
                parsingResult = FAIL;
                goto parsingFail;
            }
            if ((*(pos - 1)) != CR) { // requestLine에 'CR''LF'가 쌍으로 오지 않느다면 
                printf("[Exception] CR must come before LF. (request function)\n");
                parsingResult = FAIL;
                goto parsingFail;
            }
            // 파싱 함수 호출 ( 파싱 기준 토큰(CRLF)을 뺀 범위를 넘겨준다. (Method SP URI SP Http-version))
            parsingResult = lineParsing(start, (pos - 2), &request->line);
            parsingOrder = REQUEST_HEADER;
        }
        else if (parsingOrder == REQUEST_HEADER) {
            if ((pos - start) < 2) { //todo.
                start = pos;
                start++;
                break;
            }
            if ((*(pos - 1)) != CR) { // requestLine에 'CR''LF'가 쌍으로 오지 않느다면 
                printf("[Exception] CR must come before LF. (request function)\n");
                parsingResult = FAIL;
                goto parsingFail;
            }
            // 파싱 함수 호출 ( 파싱 기준 토큰(CRLF)을 뺀 범위를 넘겨준다. (name ":" value)
            parsingResult = headerEleParsing(start, (pos - 2), &request->header);
        }
        else {
            printf("[Exception] invalid parsing order.\n");
            parsingResult = FAIL;
            goto parsingFail;
        }
        if (parsingResult == FAIL) { // 파싱 실패
            goto parsingFail;
        }
        start = pos;
        start++; // 다음 탐색을 위해
    }
    
    // garbage save
    if (start <= end) {
#ifdef DEBUG_REQUEST_PARSING_C  
        printf("### [DEBUG] --- garbaget save start --- \n");
#endif
        parsingResult = saveTokenValue(&request->garbage, start, end);
        if (parsingResult == FAIL) {
            goto parsingFail;
        }
#ifdef DEBUG_REQUEST_PARSING_C  
        printf("### [DEBUG] --- garbaget save end --- \n");
#endif   
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request parsing end --- \n");
#endif

parsingFail:
    return parsingResult;
}

/**
 * header의 토큰을 name:value 형태로 파싱
 *
 * @param   start           파싱하려는 문자 배열의 첫번째 주소를 가르키고 있는 포인터
 * @param   end             파싱하려는 문자 배열의 마지막 주소를 가르키고 있는 포인터
 * @param   requestHeader   파싱 후 토큰들을 저장할 구조체 메모리 주소
 * @return  int             파싱 성공 여부 반환 (success 1, fail -1)
 * */
int headerEleParsing(char *start, char *end, requestHeader *requestHeader)
{  
    r_node *listTailNode = NULL;
    nameValueSet newData;
    char *pos = NULL;
    int parsingResult = 0;
    unsigned int remainedTokenSize = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(requestHeader, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);
 
#ifdef debug_request_parsing_c  
    printf("### [debug] --- query element parsing start --- \n");
#endif

    // namevalueset 초기화
    initNameValueSet(&newData);

    /* parsing */
    remainedTokenSize = ((end - start) + 1);
    pos = memchr(start, COLON, remainedTokenSize);
    if (pos == NULL) {
        parsingResult = saveTokenValue(&newData.name, start, end);
        if (parsingResult == FAIL) {
            goto parsingFail;
        }
    }
    else { // separate 가 있으면 pos 기준으로 name value에 저장
        // name ( start ~ pos - 1 )
        if ((pos - start) > 0) {
            parsingResult = saveTokenValue(&newData.name, start, (pos - 1));
            if (parsingResult == FAIL) {
                goto parsingFail;
            }
        } 
        // value ( pos + 1 ~ end )
        pos++; // 최초 SP 무시
        if ((end - pos) > 0) {
            parsingResult = saveTokenValue(&newData.value, (pos + 1), end);
            if (parsingResult == FAIL) {
                goto parsingFail;
            }
        }
    }

    // 저장된 토큰들 list에 추가 
    parsingResult = pushBackRequestNode(&requestHeader->header, &newData);
    if (parsingResult == FAIL) {
        goto parsingFail;
    }

    // header 토큰 정보 저장
    listTailNode = requestHeader->header.tail;
    if (newData.name.size == strlen("host")) {
        if (memcmp(newData.name.value, "Host", strlen("Host")) == IS_SAME) {
            requestHeader->headerElements[HOST] = &listTailNode->data.value; // Host 값 정보의 주소를 저장
        }
    }
    // ... todo. 다른 element 가 추가 되면 추가 ( Accept 등.. )

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [debug] --- query element parsing end --- \n");
#endif

parsingFail:
    
    if (parsingResult == FAIL) {
        deleteNameValueSet(&newData);
    }

    return parsingResult;
}
/**
 * request-line 파싱 함수
 *
 * @param   start           파싱하려는 문자 배열의 첫번째 주소를 가르키고 있는 포인터
 * @param   end             파싱하려는 문자 배열의 마지막 주소를 가르키고 있는 포인터
 * @param   requestLine     파싱 후 토큰들을 저장할 구조체
 * @return  int             파싱 성공 여부 반환 (SUCCESS 1, FAIL -1)
 * */
int lineParsing(char *start, char *end, requestLine *requestLine)
{
    char *pos = NULL;
    int parsingResult = 0;
    unsigned int remainedTokenSize = 0;
    lineParsingOrder parsingOrder = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(requestLine, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);
    
#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line parsing start --- \n");
#endif
  
    /* parsing */
    parsingResult = SUCCESS;
    parsingOrder = METHOD;
    while (start <= end) {
        remainedTokenSize = ((end - start) + 1);
        pos = memchr(start, SP, remainedTokenSize);
        if (pos == NULL) {
            if (parsingOrder != HTTP_VERSION) {
                printf("[Exception] no exist token. (lineParsing function) \n"); 
                parsingResult = FAIL;
                goto parsingFail;
            }
            pos = (end + 1); // todo.
        }
        if ((pos - start) <= 0) { // 현재 저장 하려는 값의 길이가 0 이거나 작다면
            printf("[Exception] request line elements is essentail element. (lineParsing function) \n");
            parsingResult = FAIL;
            goto parsingFail;
        }
        if (parsingOrder == METHOD) {
#ifdef DEBUG_REQUEST_PARSING_C 
            printf("### [DEBUG] - current order : method\n");
#endif
            parsingResult = saveTokenValue(&requestLine->method, start, (pos - 1));
            parsingOrder = URI;
        }
        else if (parsingOrder == URI) {
#ifdef DEBUG_REQUEST_PARSING_C 
            printf("### [DEBUG] - current order : uri\n");
#endif
            parsingResult = uriParsing(start, (pos - 1), &requestLine->uri); 
            parsingOrder = HTTP_VERSION;
        }
        else if(parsingOrder == HTTP_VERSION) {
#ifdef DEBUG_REQUEST_PARSING_C 
            printf("### [DEBUG] - current order : http - version\n");
#endif
            parsingResult = saveTokenValue(&requestLine->version, start, end);
            if (parsingResult == FAIL) {
                goto parsingFail;
            }
            break;
        }

        if (parsingResult == FAIL) {
            goto parsingFail;
        }
        start = pos;
        start++;
    }


#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line parsing end --- \n");
#endif

parsingFail:
    
    return parsingResult;
}

/**
 * request-line - uri 파싱 함수
 *
 * @param   start           파싱하려는 문자 배열의 첫번째 주소를 가르키고 있는 포인터
 * @param   end             파싱하려는 문자 배열의 마지막 주소를 가르키고 있는 포인터
 * @param   requestUri      파싱 후 토큰들을 저장할 구조체
 * @return  int             파싱 성공 여부 반환 (SUCCESS 1, FAIL -1) 
 * */
int uriParsing(char *start, char *end, requestUri *requestUri)
{
    char *pos = NULL;
    int parsingResult = 0;
    uriParsingOrder parsingOrder = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(requestUri, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line - uri parsing start --- \n");
#endif

    parsingResult = SUCCESS;
    parsingOrder = PATH;
   
    /* parsing */
    while (start <= end) {
        if (parsingOrder == PATH) { // Path 파싱
            pos = seekQueryFragmentToken(start, end, &parsingOrder);
            if (pos == NULL) {
                parsingResult = FAIL;
                goto parsingFail;
            }
            if ((pos - start) <= 0){ // uri의 Path는 필수 요소 이기 때문에 예외 처리
                printf("[Exception] path token is essentail element. (uriPasring function) \n");
                parsingResult = FAIL;
                goto parsingFail;
            }
            // 토큰 저장
            parsingResult = saveTokenValue(&requestUri->absolutePath, start, (pos - 1));    
        }
        else if (parsingOrder == PATH_QUERY) { // Query 파싱 (함수 호출)
            pos = seekFragmentToken(start, end, &parsingOrder);
            if (pos == NULL) {
                parsingResult = FAIL;
                goto parsingFail;
            }
            // 토큰 저장 (함수 호출)
            if ((pos - start) > 0) { // query 값이 존재 할 때, 없으면 넘어감
                parsingResult = queryParsing(start, (pos - 1), &requestUri->query);
                if (parsingResult == FAIL) {
                    goto parsingFail;
                }
            }
        }
        else if (parsingOrder == PATH_FRAGMENT) { // Fragment 파싱
            // 토큰 저장
            if ((end - start) >= 0) { // fragment 값이 존재 할 때, 없으면 넘어감
                parsingResult = saveTokenValue(&requestUri->fragment, start, end);
                if (parsingResult == FAIL) {
                    goto parsingFail;
                }
            }
            break;
        }
        else {
            printf("[Exception] invalid parsing order.\n");
            parsingResult = FAIL;
            goto parsingFail;
        }
        if (parsingResult == FAIL) {
            goto parsingFail;
        }
        start = pos;
        start++;
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line - uri parsing end --- \n");
#endif

parsingFail:

    return parsingResult;
}

/**
 * request-line - uri - qeury 파싱 함수
 *
 * @param   start       파싱하려는 문자 배열의 첫번째 주소를 가르키고 있는 포인터
 * @param   end         파싱하려는 문자 배열의 마지막 주소를 가르키고 있는 포인터
 * @param   query       파싱 후 토큰들을 저장할 링크드리스트
 * @return  int         파싱 성공 여부 반환 (SUCCESS 1 , FAIL -1)         
 * */
int queryParsing(char *start, char *end, r_list *query)
{
    char *pos = NULL;
    int parsingResult = 0;
    unsigned int remainedTokenSize = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(query, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line - uri - query parsing start --- \n");
#endif

    /* parsing */
    parsingResult = SUCCESS;
    while (start <= end) {
        remainedTokenSize = ((end - start) + 1);
        pos = memchr(start, QUERY_AND, remainedTokenSize);
        if (pos == NULL) { // 만약 '&' 이 없다면 현재 name / value 쌍을 마지막으로 저장학고 끝낸다.
            pos = (end + 1); // todo.
        }

        // element 파싱 함수 호출 (파싱 기준 토큰(&)은 뺴고 넘겨준다.)
        if ((pos - start) > 0) { // 값이 있을 때
            parsingResult = queryEleParsing(start, (pos - 1), query); 
            if (parsingResult == FAIL) {
                goto parsingFail;  
            }
        }
        start = pos;
        start++;
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line - uri - query parsing end ---\n");
#endif

parsingFail:

    return parsingResult;
}

/**
 * query 의 토큰을 name=value 형태로 파싱
 *
 * @param   start       파싱하려는 문자 배열의 첫번째 주소를 가르키고 있는 포인터
 * @param   end         파싱하려는 문자 배열의 마지막 주소를 가르키고 있는 포인터
 * @param   list        파싱 후 토큰들을 저장할 링크드리스트
 * @return  int         파싱 성공 여부 반환 (success 1, fail -1)
 * */
int queryEleParsing(char *start, char *end, r_list *list)
{  
    nameValueSet newData;
    char *pos = NULL;
    int parsingResult = 0;
    unsigned int remainedTokenSize = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(list, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);
 
#ifdef debug_request_parsing_c  
    printf("### [debug] --- query element parsing start --- \n");
#endif

    // namevalueset 초기화
    initNameValueSet(&newData);

    /* parsing */
    remainedTokenSize = ((end - start) + 1);
    pos = memchr(start, ASSIGN, remainedTokenSize);
    if (pos == NULL) {
        parsingResult = saveTokenValue(&newData.name, start, end);
        if (parsingResult == FAIL) {
            goto parsingFail;
        }
    }
    else { // separate 가 있으면 pos 기준으로 name value에 저장
        // name ( start ~ pos - 1 )
        if ((pos - start) > 0) {
            parsingResult = saveTokenValue(&newData.name, start, (pos - 1));
            if (parsingResult == FAIL) {
                goto parsingFail;
            }
        } 
        // value ( pos + 1 ~ end )
        if ((end - pos) > 0) {
            parsingResult = saveTokenValue(&newData.value, (pos + 1), end);
            if (parsingResult == FAIL) {
                goto parsingFail;
            }
        }
    }

    // 저장된 토큰들 list에 추가 
    parsingResult = pushBackRequestNode(list, &newData);
    if (parsingResult == FAIL) {
        goto parsingFail;
    }

#ifdef debug_request_parsing_c  
    printf("### [debug] --- query element parsing end --- \n");
#endif

parsingFail:
    
    if (parsingResult == FAIL) {
        deleteNameValueSet(&newData);
    }

    return parsingResult;
}

/**
 * query 과 Fragment Token 검색
 *
 * @param   start   검색하려는 토큰의 시작 주소
 * @param   end     검색 하려는 토큰의 마지막 주소
 * @param   order   검색 후 다음 순서를 저장 시킬 enum
 * @return  char    검색 후 검색 결과가 가르키는 메모리 주소 반환
 *                  실패시 NULL 반환
 * */
char *seekQueryFragmentToken(char *start, char *end, uriParsingOrder *order)
{
    unsigned int remainedTokenSize = 0;
    char *resultPos = NULL;
    char *queryPos = NULL;
    char *fragmentPos = NULL;

    /* param exception */
    PARAM_EXP_CHECK(start, NULL);
    PARAM_EXP_CHECK(end, NULL);
    PARAM_EXP_CHECK(order, NULL);
    RANGE_EXP_CHECK(start, end, NULL);
 
    /* seek */
    remainedTokenSize = ((end - start) + 1);
    queryPos = memchr(start, QUERY, remainedTokenSize);
    fragmentPos = memchr(start, FRAGMENT, remainedTokenSize);
    if (queryPos == NULL && fragmentPos == NULL) { // uri 에 query, fragment 둘다 존재 하지 않음
        resultPos = (end + 1);
    }
    else if (queryPos != NULL && fragmentPos != NULL){ // uri에 query fragment 둘다 존재
        if (queryPos > fragmentPos) { // fragment 가 query보다 먼저 올 수 없음
            printf("[Exception] fragment token cannot come before query. (seekQueryFragment function) \n");
            resultPos = NULL;
            goto seekQueryFragmentTokenFail;
        }
        (*order) = PATH_QUERY;
        resultPos = queryPos;
    }
    else if (queryPos != NULL) { // uri 에 qeury만 존재
        (*order) = PATH_QUERY;
        resultPos = queryPos;
    }
    else if (fragmentPos != NULL) { // uri 에 fragment만 존재 
        (*order) = PATH_FRAGMENT;
        resultPos = fragmentPos;
    }

seekQueryFragmentTokenFail:
    
    return resultPos;
}

/**
 * Fragment Token 검색
 *
 * @param   start   검색하려는 토큰의 시작 주소
 * @param   end     검색 하려는 토큰의 마지막 주소
 * @param   order   검색 후 다음 순서를 저장 시킬 enum
 * @return  char    검색 후 검색 결과가 가르키는 메모리 주소 반환
 *                  실패시 NULL 반환
 * */
char *seekFragmentToken(char *start, char *end, uriParsingOrder *order)
{
    unsigned int remainedTokenSize = 0;
    char *resultPos = NULL;
    char *queryPos = NULL;
    char *fragmentPos = NULL;

    /* param exception */
    PARAM_EXP_CHECK(start, NULL);
    PARAM_EXP_CHECK(end, NULL);
    PARAM_EXP_CHECK(order, NULL);
    RANGE_EXP_CHECK(start, end, NULL);

    /* seek */
    remainedTokenSize = ((end - start) + 1);
    queryPos = memchr(start, QUERY, remainedTokenSize);
    fragmentPos = memchr(start, FRAGMENT, remainedTokenSize);
    if (queryPos != NULL) { // fragment 다음으로는 query가 올 수 없음
        printf("[Exception] query token is cannot come after fragment. (seekFragmentToken) \n");
        resultPos = NULL;
        goto seekFragmentTokenFail;
    }
    if (fragmentPos == NULL) { // fragment 가 없다면 끝까지 파싱
        resultPos = (end + 1);
    }
    else { // 있다면 fragment 위치 반환
        (*order) = PATH_FRAGMENT;
        resultPos = fragmentPos;
    }

seekFragmentTokenFail:
    
    return resultPos;
}
/**
 * request 파싱 후 저장된 데이터를 보여주는 함수
 *
 * @param   request     rqeuest 파싱 결과가 저장되어 있는 구초제
 * @return  void        반환 값 없음
 *
 * */
void viewRequest(const request *request)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(request);

    /* print tokens.. */
    // method 
    printf("mehtod : ");
    viewToken(&request->line.method);
    // path
    printf("\npath : ");
    viewToken(&request->line.uri.absolutePath);
    // query
    if (request->line.uri.query.listCount == 0) {
        printf("\nquery : none\n");
    }
    else {
        printf("\nquery : \n");
        viewRequestLinkedList(&request->line.uri.query);
    }
    // fragment
    printf("fragment : ");
    viewToken(&request->line.uri.fragment);
    // version
    printf("\nversion : ");
    viewToken(&request->line.version);
    // header
    if (request->header.header.listCount == 0) {
        printf("\nheader : none\n"); 
    }
    else {
        printf("\nheader : \n");
        viewRequestLinkedList(&request->header.header);
    }
    // garbage
    printf("garbage : ");
    viewToken(&request->garbage);
    
    printf("\n");

    return ;
}
