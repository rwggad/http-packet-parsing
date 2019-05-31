#include "parsing_config.h"

/**
 * config에 저장된 요소 삭제
 *
 * @param   config  요소를 삭제할 구조체의 메모리 주소
 * @return  void    반환 값 없음
*/
void deleteConfig(configTable *configTable)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(configTable);
    
    /* reset configTable element */
    deleteConfigList(&configTable->config);
    
    return;
}

