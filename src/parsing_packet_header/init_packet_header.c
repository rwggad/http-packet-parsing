#include "parsing_packet_header.h"

/**
 * tuple5 구조체 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
*/
void initTuple5(tuple5 *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* reset tuple5 struct */
	memset(target, 0, sizeof(struct tuple5));
    
    return;
}

/**
 * session 구조체 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
*/
void initSession(session *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* reset session struct */
	memset(target, 0, sizeof(struct session));
    
    return;
}


/**
 * session table 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
*/
void initSessionTable(sessionTable *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* reset configData element */
    initSessionList(&target->sessionList);
    
    return;
}

