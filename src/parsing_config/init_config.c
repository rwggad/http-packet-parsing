#include "../../include/parsing_config.h"

/**
 * configData 초기화
 *
 * @param   token   초기화할 구조체의 메모리 주소
 * @return  void    반환 값 없음
*/
void initConfig(config *configData)
{
    /* param exception */
    if (configData == NULL) {
        printf("[Exception] param is null pointer. (initConfig function, configData)\n");
        return;
    }

    /* reset configData element */
    initConfigLinkedList(&configData->configDataList);
    
    return;
}

