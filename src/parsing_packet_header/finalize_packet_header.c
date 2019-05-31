#include "parsing_packet_header.h"

/**
 * seesionTable에 저장된 요소 삭제
 *
 * @param   taget       요소를 삭제할 구조체의 메모리 주소
 * @return  void        반환 값 없음
*/
void deleteSessionTable(sessionTable *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);
    
    /* reset configData element */
    deleteSessionList(&target->sessionList);
    
    return;
}

