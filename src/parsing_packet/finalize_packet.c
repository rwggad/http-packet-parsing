#include "parsing_packet.h"

/**
 * requestUri의 요소 중 메모리 할당된 것들을 해제 해줌 (token->value, List)
 *
 * @param   target          메모리를 해제할 요소들을 담고 있는 구조체
 * @return  void            반환 값 없음
 *
 * */
void deleteUri(requestUri *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* element allocation free */
    deleteTokenValue(&target->absolutePath);
    deleteRequestLinkedList(&target->query);
    deleteTokenValue(&target->fragment);
 
#ifdef DEBUG_REQUEST_PARSING_FINAL_C 
    printf("### [DEBUG] delete requestUri struct..\n");
#endif

    return;
}


/**
 * requestLine의 요소 중 메모리 할당된 것들을 해제 해줌 (token->value, List)
 *
 * @param   target          메모리를 해제할 요소들을 담고 있는 구조체
 * @return  void            반환 값 없음
 *
 * */
void deleteLine(requestLine *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* allocation free */
    deleteTokenValue(&target->method);
    deleteUri(&target->uri);
    deleteTokenValue(&target->version);
 
#ifdef DEBUG_REQUEST_PARSING_FINAL_C 
    printf("### [DEBUG] delete requestLine struct..\n");
#endif

    return;
}

/**
 * request header 의 요소 중 메모리 할당된 것들을 해제 해줌 (garbageList)
 *
 * @param   target      메모리를 해제할 요소들을 담고 있는 구조체
 * @return  void        반환 값 없음
 *
 * */
void deleteHeader(requestHeader *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* element allocation free */
    deleteRequestLinkedList(&target->header);

#ifdef DEBUG_REQUEST_PARSING_FINAL_C 
    printf("### [DEBUG] delete request struct..\n");
#endif

    return;
}
/**
 * request의 요소 중 메모리 할당된 것들을 해제 해줌 (token->value, List)
 *
 * @param   target      메모리를 해제할 요소들을 담고 있는 구조체
 * @return  void        반환 값 없음
 *
 * */
void deleteRequest(request *target)
{

    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* element allocation free */
    deleteLine(&target->line);
    deleteHeader(&target->header);
    deleteTokenValue(&target->garbage);

#ifdef DEBUG_REQUEST_PARSING_FINAL_C 
    printf("### [DEBUG] delete request struct..\n");
#endif

    return;
}


