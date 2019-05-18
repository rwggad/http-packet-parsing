#include "../../include/parsing_config.h"

/**
 * tokens 으로 넘어 온 값을 config 값 으로 파싱
 *
 * @param   tokens      config 파일의 값을 읽어온 값
 * @param   configData  파싱한 결과 저장랑 구조체
 * @reutrn  int         성공 여부 반환 (SUCCESS : 0 FAIL : -1)
 *
 * */
int configParsing(const token *tokens, config *configData)
{
    char *start = NULL;
    char *pos = NULL;
    char *end = NULL;
    int result = 0;
    unsigned int remainedTokenSize = 0;
    configDataSet newConfigDataSet;
    
    /* param excpetion */
    PARAM_EXP_CHECK(tokens, FAIL);
    PARAM_EXP_CHECK(configData, FAIL);

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- config parsing start ---\n");
#endif

    /* set start, end*/
    start = tokens->value; // start pointer 
    end = ((tokens->value + tokens->size) - 1); // end pointer
    
    /* parsing */
    result = SUCCESS;
    while (start <= end) { // todo. 종료조건 수정 
        initConfigDataSet(&newConfigDataSet); // configSet 초기화
        remainedTokenSize = ((end - start) + 1); // 검색 범위 지정
        pos = memchr(start, LF, remainedTokenSize); // LF 검색
        if (pos == NULL) { // LF를 찾지 못한다면
            printf("[Exception] no exist LF token. (configParsing function) \n");
            result = FAIL;
            goto configParsingFail;
        } 
        if ((pos - start) < 2) { // 유효 범위 체크 (CR, 토큰길이가 0이 아닌지?)
            // todo. exception
        }    
        if ((*(pos - 1) != CR)) {
            // todo. exception
        }
        result = configElementParsing(start, (pos - 2), &newConfigDataSet);
        if (result == FAIL) { 
            goto configParsingFail;
        } 
        pushBackConfigNode(&configData->configDataList, &newConfigDataSet); 
        start = pos;
        start++;
    }
#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- config parsing end ---\n");
    printf("---------------------------------------------\n");
    viewConfigLinkedList(&configData->configDataList);
    printf("---------------------------------------------\n");
#endif

configParsingFail:

    return result;
}

/**
 * config 값을 각각 요소에 맞게 파싱 (type, method, string, server list)
 *
 * @param   start           파싱 시작 지점
 * @param   end             파싱 끝 지점
 * @param   configDataSet   파싱한 결과 저장랑 구조체
 * @reutrn  int             성공 여부 반환 (SUCCESS : 0 FAIL : -1)
 *
 * */
int configElementParsing(char *start, char *end, configDataSet *dataSet)
{
    char *pos = NULL;
    int result = 0;
    unsigned int remainedTokenSize = 0;
    elementParsingOrder parsingOrder = 0; 

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(dataSet, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);
    
#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- config element parsing start ---\n");
#endif

    /* parsing */
    result = SUCCESS;
    parsingOrder = ELEMENT_MATCH_TYPE;
    while (start <= end) {
        remainedTokenSize = ((end - start) + 1); // 검색 범위 지정
        pos = memchr(start, SP, remainedTokenSize); // SP 검색
        if (pos == NULL) { // todo. 좀더 바꿀 수 없을까? 
            if (parsingOrder == ELEMENT_SERVER_LIST) {
                result = configServerParsing(start, end, dataSet);
                break;
            }
            printf("[Exception] no exist SP token. (confingElementParsing function) \n"); 
            result = FAIL;
            goto configElementParsingFail;  
        }

        // 토큰 값이 있는지 ?
        if ((pos - start) <= 0) {
            printf("[Exception] config elements is essentail element. \n");
            result = FAIL;
            goto configElementParsingFail;
        }

        // todo. 좀더 최적화?, 그리고 타입이 제대로 들어오는지 체크
        if (parsingOrder == ELEMENT_MATCH_TYPE) { // match type
            result = saveTokenValue(&dataSet->matchType, start, (pos - 1));
            parsingOrder = ELEMENT_MATCH_METHOD;
        }
        else if (parsingOrder == ELEMENT_MATCH_METHOD) { // match method
            result = saveTokenValue(&dataSet->matchMethod, start, (pos - 1));
            parsingOrder = ELEMENT_MATCHING_STRING;
        }
        else if (parsingOrder == ELEMENT_MATCHING_STRING) { // matching string
            result = saveTokenValue(&dataSet->matchingString, start, (pos- 1));
            parsingOrder = ELEMENT_SERVER_LIST;
        }

        if (result == FAIL) {
            goto configElementParsingFail;
        }
        start = pos;
        start++;
    }

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- config element parsing end ---\n");
#endif

configElementParsingFail:

    return result;
}

/**
 * serverList 를 각각 ',' 기준으로 파싱 
 *
 * @param   start           파싱 시작 지점
 * @param   end             파싱 끝 지점
 * @param   dataSet         파싱 결과를 저장 시킬 구조체 메모리 주소
 * @reutrn  int             성공 여부 반환 (SUCCESS : 0 FAIL : -1)
 *
 * */
int configServerParsing(char *start, char *end, configDataSet *dataSet)
{
    char *pos = NULL;
    int result = 0;
    unsigned int remainedTokenSize = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(dataSet, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- config server parsing start ---\n");
#endif

    /* parsing */
    result = SUCCESS;
    while (start <= end) {
        // COMMA 검색
        remainedTokenSize = ((end - start) + 1);
        pos = memchr(start, COMMA, remainedTokenSize);
        if (pos == NULL) { // todo. 수정
            pos = end;
        }
        if ((pos - start) <= 0) {
            // todo. continue;
        }
        if (dataSet->serverListSize >= MAX_SERVER_SIZE) {
            printf("MAX_SERVER_SIZE is (%d) !!\n", MAX_SERVER_SIZE);
            break;
        }
        // 서버 값을 저장시켜준다.
        result = saveTokenValue(&dataSet->serverList[dataSet->serverListSize], start, (pos - 1));
        if (result == FAIL) {
            goto parsingFail;
        }
        dataSet->serverListSize++;
        start = pos;
        start++;
    }

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] dataSet current size : %d\n", dataSet->serverListSize);
    printf("### [DEBUG] --- cofing server parsing end ---\n");
#endif

parsingFail:

    return result;
}
