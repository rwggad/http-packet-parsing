#include "parsing_packet.h"

/**
 * requestUri 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
 */
void initRequestUri(requestUri *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* reset requestUri elements */
    initToken(&target->absolutePath);
    initRequestLinkedList(&target->query);
    initToken(&target->fragment);

    return;
}

/**
 * requestLine 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
 */
void initRequestLine(requestLine *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* reset requestLine elements */
    initToken(&target->method);
    initRequestUri(&target->uri);
    initToken(&target->version);

    return;
}

/**
 * requestHeader 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
 */
void initRequestHeader(requestHeader *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* reset requestLine elements */
    initRequestLinkedList(&target->header);
    memset(target->headerElements, 0, sizeof(target->headerElements));
    return;
}
/**
 * Request 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
 */
void initRequest(request *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* reset request elements */
    initRequestLine(&target->line);
    initRequestHeader(&target->header);
    initToken(&target->garbage);

    return;
}

