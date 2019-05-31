#include "parsing_config.h"

/**
 * configTable 초기화
 *
 * @param   token   초기화할 구조체의 메모리 주소
 * @return  void    반환 값 없음
*/
void initConfig(configTable *configTable)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(configTable);

    /* reset configTable element */
    initConfigList(&configTable->config);
    
    return;
}

