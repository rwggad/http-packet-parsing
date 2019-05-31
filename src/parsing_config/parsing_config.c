#include "parsing_config.h"

/**
 * tokens 으로 넘어 온 값을 config 값 으로 파싱
 *
 * @param   tokens      config 파일의 값을 읽어온 값
 * @param   configData  파싱한 결과 저장랑 구조체
 * @reutrn  int         성공 여부 반환 (SUCCESS : 0 FAIL : -1)
 *
 * */
int configParsing(const token *tokens, configTable *configList)
{
    char *start = NULL;
    char *pos = NULL;
    char *end = NULL;
    int result = 0;
    unsigned int configLine = 0;
    unsigned int remainedTokenSize = 0;
    configDataSet newConfigDataSet;
    
    /* param excpetion */
    PARAM_EXP_CHECK(tokens, FAIL);
    PARAM_EXP_CHECK(configList, FAIL);

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- config parsing start ---\n");
#endif

    /* set start, end*/
    start = tokens->value; // start pointer 
    end = ((tokens->value + tokens->size) - 1); // end pointer
   

    /* parsing */
    result = SUCCESS;
    while (start <= end) { 
        initConfigDataSet(&newConfigDataSet); // configSet 초기화
        remainedTokenSize = ((end - start) + 1); // 검색 범위 지정
        pos = memchr(start, LF, remainedTokenSize); // LF 검색
        if (pos == NULL) { // LF를 찾지 못한다면
            printf("[Exception] no exist LF token. (configParsing function) \n");
            result = FAIL;
            goto parsingFail;
        } 
        configLine++;
        if ((pos - start) < 2) { // 유효 범위 체크 (CR, 토큰길이가 0이 아닌지?)
            printf("current line no value or no exist CR token, skip (config line : %d)\n", configLine);
            start = pos;
            start++;
            continue;
        }    
        if ((*(pos - 1) != CR)) { // CRLF 의 쌍이 아니라면 현재 config line은 skip 한다.
            printf("CR must come before LF. skip (config line : %d)\n", configLine);
            start = pos;
            start++;
            continue;
        }
        result = configElementParsing(start, (pos - 2), &newConfigDataSet); // 현재 confing line을 파싱 
        if (result == SUCCESS) { 
            pushBackConfigNode(&configList->config, &newConfigDataSet); // 파싱 결과를 저장 
        }
        else {
            printf(" skip current line (config line : %d)\n", configLine);
        } 
        start = pos;
        start++;
    }

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- config parsing end ---\n");
#endif

parsingFail:

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
    int result = 0;
    char *pos = NULL;
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
        if (pos == NULL) { 
            if (parsingOrder != ELEMENT_SERVER_LIST) {
                printf("[Exception] no exist SP token. (confingElementParsing function) \n"); 
                result = FAIL;
                goto parsingFail; 
            }        
        }
        
        if (parsingOrder == ELEMENT_SERVER_LIST) { // server list 
            result = configServerParsing(start, end, dataSet); // server list
            if (result == FAIL) {
                goto parsingFail;
            }
            break;
        }

        // 토큰 값이 있는지 ?
        if ((pos - start) <= 0) {
            printf("[Exception] config elements is essentail element. \n");
            result = FAIL;
            goto parsingFail;
        }
        
        if (parsingOrder == ELEMENT_MATCH_TYPE) { // match type
            if (mem_str_i_cmp(start, (pos - 1), "PATH") == IS_SAME) { // 메모리의 start ~ (end - 1) 범위의 값과 문자열 PATH가 동일한지?
                dataSet->type = TYPE_PATH;
            }
            else if (mem_str_i_cmp(start, (pos - 1), "HOST") == IS_SAME) {
                dataSet->type = TYPE_HOST;
            }
            else {
                printf("[Exception] unacceptable match type.\n");
                result = FAIL;
                goto parsingFail;
            }
            parsingOrder = ELEMENT_MATCH_METHOD;
        }
        else if (parsingOrder == ELEMENT_MATCH_METHOD) { // match method
            if (mem_str_i_cmp(start, (pos - 1), "ANY") == IS_SAME) {
                dataSet->method = METHOD_ANY;
            }
            else if (mem_str_i_cmp(start, (pos - 1), "START") == IS_SAME) {
                dataSet->method = METHOD_START;
            }
            else if (mem_str_i_cmp(start, (pos - 1), "END") == IS_SAME) {
                dataSet->method = METHOD_END;
            }
            else {
                printf("[Exception] unacceptable match method.\n");
                result = FAIL;
                goto parsingFail;
            }
            parsingOrder = ELEMENT_MATCHING_STRING;
        }
        else if (parsingOrder == ELEMENT_MATCHING_STRING) { // matching string
            result = saveTokenValue(&dataSet->matchingString, start, (pos - 1));
            parsingOrder = ELEMENT_SERVER_LIST;
        }
        else {
            printf("[Exception] parsing order exception.\n");
            result = FAIL;
            goto parsingFail;
        }

        if (result == FAIL) {
            goto parsingFail;
        }
        start = pos;
        start++;
    }

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- config element parsing end ---\n");
#endif

parsingFail:

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
    int result = 0;
    char *pos = NULL;
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
        if (dataSet->serverListSize >= MAX_SERVER_SIZE) { // server list에 남은 공간이 없다면
            printf("server list max size is (%d). no more svae server information \n", MAX_SERVER_SIZE);
            break;
        }
        remainedTokenSize = ((end - start) + 1);
        pos = memchr(start, COMMA, remainedTokenSize);
        if (pos == NULL) { 
            result = configIpPortParsing(start, end, &dataSet->serverList[dataSet->serverListSize]);
            if (result == SUCCESS) { // 파싱 성공시 다음 배열에 저장하기 위해 index++
                dataSet->serverListSize++;
            }
            break;
        }
        if ((pos - start) > 0) { // 현재 ip 값이 있다면
            result = configIpPortParsing(start, (pos - 1), &dataSet->serverList[dataSet->serverListSize]);
            if (result == SUCCESS) { // 파싱 성공시 다음 배열에 저장하기 위해 index++
                dataSet->serverListSize++;
            }
        }
        start = pos;
        start++;
    }

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- cofing server parsing end ---\n");
#endif

    return result;
}

/**
 * ip:host 파싱
 *
 * @param   start           파싱 시작 지점
 * @param   end             파싱 끝 지점
 * @param   server          파싱 결과를 저장 시킬 구조체 메모리 주소
 * @reutrn  int             성공 여부 반환 (SUCCESS : 0 FAIL : -1)
 *
 * */
int configIpPortParsing(char *start, char *end, ipSet *server)
{
    char *pos = NULL;
    int result = 0;
    int portNumber = 0;
    unsigned int remainedTokenSize = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(server, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);

#ifdef DEBUG_CONFIG_PARSING_C
    printf("### [DEBUG] --- ip:port parsing start ---\n");
#endif

    /* parsing */
    result = SUCCESS;
    
    // COLON 검색
    remainedTokenSize = ((end - start) + 1);
    pos = memchr(start, COLON, remainedTokenSize);
    if (pos == NULL) { 
        printf("server type is (ipv4 or ipv6):port, current no exist port, skip current server \n");
        result = FAIL;
        goto parsingFail;
    }
    if ((pos - start) <= 0) { // ip 값 : 기준으로 검사
        result = FAIL;
        goto parsingFail;
    }
    if (((end + 1) - pos) <= 0) { // port 값 : 기준으로 검사
        printf("current server no exist port number. skip current server\n");
        result = FAIL;
        goto parsingFail;
    }
    // set ip
    if (mem_inet_pton(start, (pos - 1), &server->addr) == FAIL) {
        result = FAIL;
        goto parsingFail;
    }
    
    // port save
    portNumber = mem_atoi((pos + 1), end);
    if (portNumber == FAIL) {
        printf("port value parsing fail (atoi function..), skip current ip\n");
        result = FAIL;
        goto parsingFail;
    }
    if (portNumber < MIN_PORT_NUMBER_RANGE || portNumber > MAX_PORT_NUMBER_RANGE) {
        printf("port number range is (%d~ %d), skip current ip\n", MIN_PORT_NUMBER_RANGE, MAX_PORT_NUMBER_RANGE);
        result = FAIL;
        goto parsingFail;
    }
    server->addr.sin_port = portNumber;
               
#ifdef DEBUG_CONFIG_PARSING_C

    if (server->addr.sin_family == AF_INET6) {
        char srcIpAddr[INET6_ADDRSTRLEN] = {0, };
        inet_ntop(server->addr.sin_family, &server->addr.sin_addr.s_addr, srcIpAddr, sizeof(srcIpAddr));
        printf(" -ipv6 (%u) \n", server->addr.sin_addr.s_addr);
        printf(" -src ip : %s \n", srcIpAddr);
    } 
    else {
        char srcIpAddr[INET_ADDRSTRLEN] = {0, };
        inet_ntop(server->addr.sin_family, &server->addr.sin_addr.s_addr, srcIpAddr, sizeof(srcIpAddr));
        printf(" -ipv4 (%u) \n", server->addr.sin_addr.s_addr);
        printf(" -src ip : %s \n", srcIpAddr);
    }
    printf(" - port : %d\n", server->addr.sin_port);
    printf("### [DEBUG] --- ip:port parsing end ---\n");
#endif

parsingFail:

    return result;
}
