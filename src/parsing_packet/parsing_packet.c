#include "../../include/parsing_packet.h"

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
    unsigned int separateTokenSize = 0;
    requestParsingOrder parsingOrder = 0;

    /* param exception */
    if (tokens == NULL) {
        printf("[Exception] param is null pointer. (request function, token) \n");
        return FAIL;
    }
    if (request == NULL) {
        printf("[Exception] param is null pointer. (request function, request) \n");
        return FAIL;
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request parsing start --- \n");
#endif

    start = (tokens->value); // set pointer
    end = (tokens->value + tokens->size); // set pointer
    parsingResult = SUCCESS;
    parsingOrder = REQUEST_LINE;
    
    /* parsing */
    while (parsingOrder != REQUEST_PARSING_END) {
        separateTokenSize = 2; // 파싱 기준 토큰 크기 (CRLF)
        // CRLF 검색
        remainedTokenSize = ((end - start) + 1); // start에서 검색할 바이트 범위 설정
        pos = memchr(start, LF, remainedTokenSize); // LF 검색
        if (pos == NULL) { // LF를 찾지 못한다면
            printf("[Exception] no exist LF token. (request function) \n");
            parsingResult = FAIL;
            goto requestParsingFail;
        }
        if (start > (pos - 1)) { // CR을 체크 하기 위한 유효범위 체크
            printf("[Exception] (pos - 1) is out of range. (request function)\n");
            parsingResult = FAIL;
            goto requestParsingFail;
        }
        //todo. 예외 수정
        if ((*(pos - 1)) != CR) { // LF 가 있는데 CR이 없다면 예외
            printf("[Exception] CR token next must be LF token. (request function) \n");
            parsingResult = FAIL;
            goto requestParsingFail;
        }

        // request-line 파싱 (함수 호출)
        if (parsingOrder == REQUEST_LINE) {
            parsingOrder = REQUEST_HEADER;
            // line 값이 없으면 예외
            if (start > (pos - separateTokenSize)) { // 현재 pos에서 -2 만큼 한값이 start보다 작아지면 line값이 없음
                printf("[Exception] request-line is essentail element. (request function)\n");
                parsingResult = FAIL;
                goto requestParsingFail;
            }
           
            // 파싱 함수 호출 ( 파싱 기준 토큰(CRLF)을 뺀 범위를 넘겨준다. (Method SP URI SP Http-version))
            parsingResult = lineParsing(start, (pos - separateTokenSize), &request->line);
            if (parsingResult == FAIL) { // 파싱 실패
                goto requestParsingFail;
            }
            start = pos;
            start++; // 다음 탐색을 위해
            continue;
        }


        // requset-header 파싱 (함수 호출)
        if(parsingOrder == REQUEST_HEADER) {
            // header 값이 없으면 넘어감
            if (start > (pos - separateTokenSize)) { // 현재 pos에서 -2 만큼 한값이 start 보다 작아지면 header 값이 없음
                start = pos;
                start += separateTokenSize;
                parsingOrder = REQUEST_PARSING_END;
                continue;
            }

            if ((pos + 2) > end) { // 다음 CRLF가 또 있는지 체크하기 위한 유효범위 체크
                printf("[Exception] (pos + 2) is out of range. (request function) \n");
                parsingResult = FAIL;
                goto requestParsingFail;
            }
            if ((*(pos + 1)) == CR && (*(pos + 2)) == LF){ // CRLF CRLF 마지막 헤더 값임
                parsingOrder = REQUEST_PARSING_END;
            }

            // 파싱 함수 호출 ( 파싱 기준 토큰(CRLF)을 뺀 범위를 넘겨준다. (name ":" value)
            parsingResult = elementParsing(start, (pos - separateTokenSize), COLON, &request->header);  
            if (parsingResult == FAIL) {
                goto requestParsingFail;
            }
            if (parsingOrder == REQUEST_PARSING_END) {
                start += separateTokenSize; // 마지막 파싱일 경우 CRLF 만큼 더 이동 
            }
            start = pos;
            start++; 
            continue;
        }
    }

    // garbage save
    if (start < end) {

#ifdef DEBUG_REQUEST_PARSING_C  
        printf("### [DEBUG] --- garbaget save start --- \n");
#endif

        parsingResult = saveTokenValue(&request->garbage, start, end);
        if (parsingResult == FAIL) {
            goto requestParsingFail;
        }

#ifdef DEBUG_REQUEST_PARSING_C  
        printf("### [DEBUG] --- garbaget save end --- \n");
#endif   
    
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request parsing end --- \n");
#endif

requestParsingFail:
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
    unsigned int separateTokenSize = 0;
    lineParsingOrder parsingOrder = 0;

    /* param exception */
    if (start == NULL) {
        printf("[Exception] param is null pointer. (lintParsing function, start)\n");
        return FAIL;
    }
    if (end == NULL) {
        printf("[Exception] param is null pointer. (lineParsing function, end)\n");
        return FAIL;
    }
    if (start > end) {
        printf("[Exception] start pointer must be smaller than end pointer. (lineParsing function, start > end) \n");
        return FAIL;
    }
    if (requestLine == NULL) {
        printf("[Exception] param is null pointer. (lineParsing function, requestLine)\n");
        return FAIL;
    }
    
#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line parsing start --- \n");
#endif
    parsingResult = SUCCESS;
    parsingOrder = METHOD;
   
    /* parsing */
    while (parsingOrder != LINE_PARSING_END) {
        separateTokenSize = 1;
        // SP, CRLF 검색
        remainedTokenSize = ((end - start) + 1); // start에서 검색할 범위 설정
        pos = memchr(start, SP, remainedTokenSize);
        if (pos == NULL) { // 만약 SP 를 찾지 못하고 version 파싱 차례가 아니면 예외 처리,
            if (parsingOrder != HTTP_VERSION){  
                printf("[Exception] no exist token. (lineParsing function) \n"); 
                parsingResult = FAIL;
                goto lineParsingFail;
            }
            separateTokenSize = 0; // 찾은 파싱 기준 토큰을 찾지 못했기 때문에 0
            pos = end; // http-version 파싱은 끝까지 파싱
        }

        // Method 파싱 (파싱 기준 토큰을 제외하고 넘겨준다. SP제외)
        if (parsingOrder == METHOD) {
            parsingOrder = URI;
            if (start > (pos - separateTokenSize)) { // Method 는 필수요소
                printf("[Exception] method is essentail element. (lineParsing function) \n");
                parsingResult = FAIL;
                goto lineParsingFail;
            }
            // 토큰 저장
            parsingResult = saveTokenValue(&requestLine->method, start, (pos - separateTokenSize));
            if (parsingResult == FAIL) {
                goto lineParsingFail;
            }
            // todo. #define 또는 inline  처리 
            if ((pos + separateTokenSize) > end)  { // 다음으로 파싱 할 값이 없다면
                parsingOrder = LINE_PARSING_END;
                continue;
            }
            start = pos;
            start++;
            continue;
        }

        // uri 파싱 (함수 호출, 파싱 기준 토큰을 제외하고 넘겨준다. SP제외)
        if (parsingOrder == URI) { 
            parsingOrder = HTTP_VERSION;
            if (start > (pos - separateTokenSize)) { // uri 는 필수요소
                printf("[Exception] uri is essentail element. (lineParsing function) \n");
                parsingResult = FAIL;
                goto lineParsingFail;
            }
            // 함수호출
            parsingResult = uriParsing(start, (pos - separateTokenSize), &requestLine->uri); 
            if (parsingResult == FAIL) {
                goto lineParsingFail;
            }
            if ((pos + separateTokenSize) > end)  { // 다음으로 파싱 할 값이 없다면
                parsingOrder = LINE_PARSING_END;
                continue;
            }
            start = pos;
            start++;
            continue; 
        }
        
        // http-version 파싱
        if (parsingOrder == HTTP_VERSION) {
            parsingOrder = LINE_PARSING_END;
            if (start > end) { // http - version 는 필수요소
                printf("[Exception] http-version is essentail element. (lineParsing function) \n");
                parsingResult = FAIL;
                goto lineParsingFail;
            }
            parsingResult = saveTokenValue(&requestLine->version, start, end);
            if (parsingResult == FAIL) {
                goto lineParsingFail;
            }
            continue;
        }
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line parsing end --- \n");
#endif

lineParsingFail:
    
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
    unsigned int separateTokenSize = 0;
    uriParsingOrder parsingOrder = 0;

    /* param exception */
    if (start == NULL) {
        printf("[Exception] param is null pointer. (uriParsing function, start)\n");
        return FAIL;
    }
    if (end == NULL) {
        printf("[Exception] param is null pointer. (uriParsing function, end)\n");
        return FAIL;
    }
    if (start > end) {
        printf("[Exception] start pointer must be smaller than end pointer. (uriParsing function, start > end) \n");
        return FAIL;
    }
    if (requestUri == NULL) {
        printf("[Exception] param is null pointer. (uriParsing function, requestUri)\n");
        return FAIL;
    } 

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line - uri parsing start --- \n");
#endif

    parsingResult = SUCCESS;
    parsingOrder = PATH;
   
    /* parsing */
    while (parsingOrder != URI_PARSING_END) {
        // Path 파싱
        if (parsingOrder == PATH) { 
            pos = seekQueryFragmentToken(start, end, &parsingOrder, &separateTokenSize);
            if (pos == NULL) {
                parsingResult = FAIL;
                goto uriParsingFail;
            }
            if (start > (pos - separateTokenSize)) { // uri의 Path는 필수 요소 이기 때문에 예외 처리
                printf("[Exception] path token is essentail element. (uriPasring function) \n");
                parsingResult = FAIL;
                goto uriParsingFail;
            }

            // 토큰 저장
            parsingResult = saveTokenValue(&requestUri->absolutePath, start, (pos - separateTokenSize));
            if (parsingResult == FAIL) {
                goto uriParsingFail;
            }
            if ((pos + separateTokenSize) > end)  { // 다음으로 파싱 할 값이 없다면
                parsingOrder = URI_PARSING_END;
                continue;
            }
            start = pos;
            start++;
            continue;
        }

        // Query 파싱 (함수 호출)
        if (parsingOrder == PATH_QUERY) {
            pos = seekFragmentToken(start, end, &parsingOrder, &separateTokenSize);
            if (pos == NULL) {
                parsingResult = FAIL;
                goto uriParsingFail;
            }
        
            // 토큰 저장 (함수 호출)
            if (start <= (pos - separateTokenSize)) { // query 값이 존재 할 때, 없으면 넘어감
                parsingResult = queryParsing(start, (pos - separateTokenSize), &requestUri->query);
                if (parsingResult == FAIL) {
                    goto uriParsingFail;
                }
                if ((pos + separateTokenSize) > end)  { // 다음으로 파싱 할 값이 없다면
                    parsingOrder = URI_PARSING_END;
                    continue;
                }
                start = pos;
                start++;
            }
            continue;
        }
        // Fragment 파싱
        if (parsingOrder == PATH_FRAGMENT) {
            // 토큰 저장
            if (start <= end) { // fragment 값이 존재 할 때, 없으면 넘어감
                parsingResult = saveTokenValue(&requestUri->fragment, start, end);
                if (parsingResult == FAIL) {
                    goto uriParsingFail;
                }
            }
            parsingOrder = URI_PARSING_END;
            continue;
        } 
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line - uri parsing end --- \n");
#endif

uriParsingFail:

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
    unsigned int separateTokenSize = 0;
    queryParsingOrder parsingOrder = 0;

    /* param exception */
    if (start == NULL) {
        printf("[Exception] param is null pointer. (queryParsing function, start)\n");
        return FAIL;
    }
    if (end == NULL) {
        printf("[Exception] param is null pointer. (queryParsing function, start)\n");
        return FAIL;
    }
    if (start > end) {
        printf("[Exception] start pointer must be smaller than end pointer. (queryParsing function, start > end) \n");
        return FAIL;
    }
    if (query == NULL) {
        printf("[Exception] param is null pointer. (queryParsing functino, query)\n");
        return FAIL;
    } 

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line - uri - query parsing start --- \n");
#endif

    parsingResult = SUCCESS;
    parsingOrder = QUERY_ELEMENT;  

    /* parsing */
    while (parsingOrder != QUERY_PARSING_END) {
        separateTokenSize = 1;
        parsingOrder = QUERY_ELEMENT; 
        if ((*start) == QUERY_AND) { // start 가 '&' 이면 쿼리의 값이 없기 때문에 스킵
            if ((start + separateTokenSize) > end) { // 다음 파싱할 값이 있는지 유효범위 체크
                parsingOrder = QUERY_PARSING_END; // 더이상 파싱 할 값이 없다면 
                continue;
            }       
            start++;
        } 

        // Query AND 탐색
        remainedTokenSize = ((end - start) + 1);
        pos = memchr(start, QUERY_AND, remainedTokenSize);
        if (pos == NULL) { // 만약 '&' 이 없다면 현재 name / value 쌍을 마지막으로 저장학고 끝낸다.
            parsingOrder = QUERY_PARSING_END; // 마지막 파싱
            separateTokenSize = 0; // 찾는 파싱 기준 토큰이 없기 때문에 0
            pos = end; // start 에서 end 까지 
        }

        // element 파싱 함수 호출 (파싱 기준 토큰(&)은 뺴고 넘겨준다.)
        parsingResult = elementParsing(start, (pos - separateTokenSize), ASSIGN, query); 
        if (parsingResult == FAIL) {
            goto queryParsingFail;  
        }
        if ((pos + separateTokenSize) > end) { // 다음으로 파싱할 값이 있는지 유효범위 체크
            parsingOrder = QUERY_PARSING_END;
            continue;
        }
        start = pos;
        start++;
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- request - line - uri - query parsing end ---\n");
#endif

queryParsingFail:

    return parsingResult;
}

/**
 * 주어진 토큰을 매개변수로 들어온 separate 에 맞게 name, value 형태로 분리
 *
 * @param   start       파싱하려는 문자 배열의 첫번째 주소를 가르키고 있는 포인터
 * @param   end         파싱하려는 문자 배열의 마지막 주소를 가르키고 있는 포인터
 * @param   separate    분리를 시킬 기준
 * @param   list        파싱 후 토큰들을 저장할 링크드리스트
 * @return  int         파싱 성공 여부 반환 (SUCCESS 1, FAIL -1)
 * */
int elementParsing(char *start, char *end, const char separate, r_list *list)
{   
    nameValueSet newData;
    char *pos = NULL;
    int parsingResult = 0;
    unsigned int remainedTokenSize = 0;
    unsigned int separateTokenSize = 0;

    /* param exception */
    if (start == NULL) {
        printf("[Exception] param is null pointer. (elemetParing function, start)\n");
        return FAIL;
    }
    if (end == NULL) {
        printf("[Exception] param is null pointer. (elementParsing function, start)\n");
        return FAIL;
    }
    if (start > end) {
        printf("[Exception] start pointer must be smaller than end pointer. (elementParsing function, start > end) \n");
        return FAIL;
    }
    if (list == NULL) {
        printf("[Exception] param is null pointer. (elementParsing function, list)\n");
        return FAIL;
    }
    
 
#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- element parsing start --- \n");
#endif

    // nameValueSet 초기화
    initNameValueSet(&newData);

    /* parsing */
    remainedTokenSize = ((end - start) + 1);
    pos = memchr(start, separate, remainedTokenSize);
    if (pos == NULL) { // separate 가 없으면 name에 start ~ end 저장
        parsingResult = saveTokenValue(&newData.name, start, end);
        if (parsingResult == FAIL) {
            goto elementParsingFail;
        }  
    }
    else { // separate 가 있으면 pos 기준으로 name value에 저장
        separateTokenSize = 1; 
        // name ( start ~ pos - 1 )
        if (start <= (pos - separateTokenSize)) {            
            parsingResult = saveTokenValue(&newData.name, start, (pos - separateTokenSize));
            if (parsingResult == FAIL) {
                goto elementParsingFail;
            } 
        } 
        // value ( pos + 1 ~ end )
        if ((pos + separateTokenSize) <= end) {
            /*
             * todo.
             *
             * 이 부분은 header의 name 과 value를 구분해서 파싱할 때
             * value에 앞쪽에 공백을 처리 하지 못한 문제를 위해 넣어놨다. 나중에 해결 해야할 문제
             *
             * --------------------------------------------------------------------------------------
             * #rule    =   (*LWS element *(*LWS "," *LWS element))
             * LWS      =   [CRLF] 1*( SP | HT )
             *
             * 여기서 각 header의 value가 여러개가 나올 수 있는 경우는 대개 #으로 정의가 되어있다.
             *
             * ex)
             *  Accept         = "Accept" ":" #( media-range [ accept-params ] )
             *
             * 그렇기에 Accept가 나오고 ':' 가 나오고 SP가 한칸나오고, value값 하나가 나오고 그리고 
             * 또 다른 value값이 있으면 ',' 가 나오고 SP가 한칸 나오고 또다른 value값이 나오는 형태
             *
             * 현재 프로그램의 요구사항의 출력은 value값들을 각각 파싱 하는 것이 아니라 한번에 하므로 
             *
             * ex)
             *  1. name = "Accept", value = "text/html, application/xhtml+xml..
             *
             * 위 처럼 value의 앞의 공백 SP는 무시하고 저장을 한다. 그렇기에 임시로 아래처럼 SP는 무시하도록 했다.
             * ------------------------------------------------------------------------------------------
             * */
            if ((*(pos + separateTokenSize)) == SP) {
                pos++;
            }
            parsingResult = saveTokenValue(&newData.value, (pos + separateTokenSize), end);
            if (parsingResult == FAIL) {
                goto elementParsingFail;
            }
        }
    }

    // 저장된 토큰들 list에 추가 
    parsingResult = pushBackRequestNode(list, &newData);
    if (parsingResult == FAIL) {
        goto elementParsingFail;
    }

#ifdef DEBUG_REQUEST_PARSING_C  
    printf("### [DEBUG] --- element parsing end --- \n");
#endif

elementParsingFail:
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
char *seekQueryFragmentToken(char *start, char *end, uriParsingOrder *order, unsigned int *separateTokenSize)
{
    unsigned int remainedTokenSize = 0;
    char *resultPos = NULL;
    char *queryPos = NULL;
    char *fragmentPos = NULL;

    /* param exception */
    if (start == NULL) {
        printf("[Exception] param is null pointer. (seekQueryFragmentToken function, start)\n");
        return NULL;
    }
    if (end == NULL) {
        printf("[Exception] param is null pointer. (seekQueryFragmentToken function, start)\n");
        return NULL;
    }
    if (start > end) {
        printf("[Exception] start pointer must be smaller than end pointer. (seekQueryFragment function, start > end) \n");
        return NULL;
    }
    if (order == NULL) {
        printf("[Exception] param is null pointer. (seekQueryFragmentToken function, order)\n");
        return NULL;
    }
    if (separateTokenSize == NULL) {
        printf("[Exception] param is null pointer. (seekQueryFragmentToken function, seekQueryFragmentToken)\n");
        return NULL;
    }

    /* seek */
    remainedTokenSize = ((end - start) + 1);
    queryPos = memchr(start, QUERY, remainedTokenSize);
    fragmentPos = memchr(start, FRAGMENT, remainedTokenSize);
    if (queryPos == NULL && fragmentPos == NULL) { // uri 에 query, fragment 둘다 존재 하지 않음
        (*order) = URI_PARSING_END;
        (*separateTokenSize) = 0;
        resultPos = end;
    }
    else if (queryPos != NULL && fragmentPos != NULL){ // uri에 query fragment 둘다 존재
        if (queryPos > fragmentPos) { // fragment 가 query보다 먼저 올 수 없음
            printf("[Exception] fragment token cannot come before query. (seekQueryFragment function) \n");
            resultPos = NULL;
            goto seekQueryFragmentTokenFail;
        }
        (*order) = PATH_QUERY;
        (*separateTokenSize) = 1;
        resultPos = queryPos;
    }
    else if (queryPos != NULL) { // uri 에 qeury만 존재
        (*order) = PATH_QUERY;
        (*separateTokenSize) = 1;
        resultPos = queryPos;
    }
    else if (fragmentPos != NULL) { // uri 에 fragment만 존재 
        (*order) = PATH_FRAGMENT;
        (*separateTokenSize) = 1;
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
char *seekFragmentToken(char *start, char *end, uriParsingOrder *order, unsigned int *separateTokenSize)
{
    unsigned int remainedTokenSize = 0;
    char *resultPos = NULL;
    char *queryPos = NULL;
    char *fragmentPos = NULL;

    /* param exception */
    if (start == NULL) {
        printf("[Exception] param is null pointer. (seekFragmentToken, start)\n");
        return NULL;
    }
    if (end == NULL) {
        printf("[Exception] param is null pointer..! (seekFramgentToken, start)\n");
        return NULL;
    }
    if (start > end) {
        printf("[Exception] start pointer must be smaller than end pointer. (seekFragmentToken, start > end) \n");
        return NULL;
    }
    if (order == NULL) {
        printf("[Exception] param is null pointer. (seekQueryFragmentToken function, order)\n");
        return NULL;
    }
    if (separateTokenSize == NULL) {
        printf("[Exception] param is null pointer. (seekQueryFragmentToken function, seekQueryFragmentToken)\n");
        return NULL;
    }

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
        (*order) = URI_PARSING_END;
        (*separateTokenSize) = 0;
        resultPos = end;
    }
    else { // 있다면 fragment 위치 반환
        (*order) = PATH_FRAGMENT;
        (*separateTokenSize) = 1;
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
    if (request == NULL) {
        printf("[Exception] param is null pointer. (viewRequset function, request)\n");
        return;
    }

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
    if (request->header.listCount == 0) {
        printf("\nheader : none\n"); 
    }
    else {
        printf("\nheader : \n");
        viewRequestLinkedList(&request->header);
    }
    // garbage
    printf("garbage : ");
    viewToken(&request->garbage);
    
    printf("\n");

    return ;
}
