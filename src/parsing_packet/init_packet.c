#include "../../include/parsing_packet.h"

/**
 * requestUri 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
*/
void initRequestUri(requestUri *target)
{
    if (target == NULL) {
        printf("[Exception] param is null pointer. (initRequestUri function, target)\n");
        return;
    }

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
void initReqestLine(requestLine *target)
{
    if (target == NULL) {
        printf("[Exception] param is null pointer. (initRequestLine function, target)\n");
        return;
    }
    
    /* reset requestLine elements */
    initToken(&target->method);
    initRequestUri(&target->uri);
    initToken(&target->version);

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
    if (target == NULL) {
        printf("[Exception] param is null pointer. (initRequest function, target)\n");
        return;
    }
    
    /* reset request elements */
	initReqestLine(&target->line);
	initRequestLinkedList(&target->header);
	initToken(&target->garbage);

	return;
}

