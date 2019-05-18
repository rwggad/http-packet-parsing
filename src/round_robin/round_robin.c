#include "../../include/round_robin.h"

/**
 * 현재 http packet과 config 목록들 중에서 매칭되는 것이 있는지 확인 후
 * 매칭되는 config 파일의 서버 스케쥴링
 *
 * @param   packet      http request packet
 * @param   configList  config 파일 목록
 * @return  int         성공시 SUCCESS(0) 실패시 FAIL(-1) 반환
 * */
int matchCheck(const request *packet, const config *configList)
{
    int matchCheckResult = 0;
    unsigned int serverRunNowIndex = 0;
    unsigned int serverListSize = 0;
    token *headerHostValueToken = NULL;
    c_node *currentConfigNode = NULL;

    /* param exception */
    PARAM_EXP_CHECK(packet, FAIL);
    PARAM_EXP_CHECK(configList, FAIL);
    
    if (configList->configDataList.listCount == 0) {
#ifdef DEBUG_ROUND_ROBIN_C
        printf("config data list is empty!\n");
#endif
        return FAIL;
    }

#ifdef DEBUG_ROUND_ROBIN_C
    printf("### [DEBUG] --- matching check start ---\n");
#endif

    /* match typei (HOST or PATH) */
    matchCheckResult = FAIL;
    currentConfigNode = configList->configDataList.head; // config list의 노드
    while (currentConfigNode != NULL) { // config list 노드를 다 읽을 때 까지 반복
        // compareTokenAndString("비교할 토큰", "비교할 문자열", "대소문자 구별 여부")
        if (compareTokenAndString(&currentConfigNode->data.matchType, "PATH", false) == IS_SAME){ // 현재 config 값의 type이 PATH 라면?
#ifdef DEBUG_ROUND_ROBIN_C
            printf("### [DEBUG] - current type : PATH\n");
#endif           
            // path에 저장된 토큰과, 비교할 config의 문자열(match string)  그리고 비교 방법을 넘겨주면서 같은지 체크
            matchCheckResult = matchStringCheck(&packet->line.uri.absolutePath, 
                                                &currentConfigNode->data.matchingString, 
                                                &currentConfigNode->data.matchMethod);
            if (matchCheckResult == SUCCESS) { // 같다면 종료
                break;
            }
            currentConfigNode = currentConfigNode->next; // 다음 노드 확인
            continue;
        }
        else if (compareTokenAndString(&currentConfigNode->data.matchType, "HOST", false) == IS_SAME){ // 현재 config 값의 type의 HOST 라면?
#ifdef DEBUG_ROUND_ROBIN_C
            printf("### [DEBUG] - current type : HOST\n");
#endif           
            // 현재 packet 의 header에 HOST 값이 있는지 체크 후 있다면 해당 host header요소의 value token의 주소값을 받아옴
            headerHostValueToken = searchHeaderHost(&packet->header);            
            if (headerHostValueToken == NULL) { // 없다면 건너뜀
                currentConfigNode = currentConfigNode->next;
                continue;
            }
#ifdef DEBUG_ROUND_ROBIN_C
            printf("return value token : [");
            viewToken(headerHostValueToken);
            printf("]\n");
#endif           
            // host의 value토큰 값과, 비교할 config 문자열(match string), 그리고 비교 방법을 넘겨주면서 같은지 체크
            matchCheckResult = matchStringCheck(headerHostValueToken, 
                                                &currentConfigNode->data.matchingString, 
                                                &currentConfigNode->data.matchMethod);    
            if (matchCheckResult == SUCCESS) { // 같다면 종료
                break;
            }
            currentConfigNode = currentConfigNode->next; // 다음 노드 확인
            continue;
        }
        else {// config 의 match typed이 HOST, PATH가 아니라면 
            printf("[Exception] match type is only host or path! (current :");
            viewToken(&currentConfigNode->data.matchType);
            printf(")\n");
            matchCheckResult = FAIL;
            goto matchCheckFail;
        }
    }
   
    /* 매칭되는 값을 찾았다면 현재 실행 되야할 서버 출력 */
    if (matchCheckResult == SUCCESS) {
        printf("run server.. \n");
        printf("[");
        serverRunNowIndex = currentConfigNode->data.runTimeIndex; // 현재 config에서 실행 할 서버를 가르키는 인덱스 가져옴
        serverListSize = currentConfigNode->data.serverListSize; // 현재 config에 저장된 서버들의 크기

        viewToken(&currentConfigNode->data.serverList[serverRunNowIndex]); // 서버 ip 출력
        if (serverRunNowIndex >= (serverListSize - 1)) { // 마지막 서버라면 처음으로
            currentConfigNode->data.runTimeIndex = 0;
        }
        else {
            currentConfigNode->data.runTimeIndex++;
        }
        printf("]\n");

#ifdef DEBUG_ROUND_ROBIN_C
    printf("### [DEBUG] current config rumtime index : %d \n", currentConfigNode->data.runTimeIndex);
#endif

    }

#ifdef DEBUG_ROUND_ROBIN_C
    printf("### [DEBUG] --- matching check end ---\n");
#endif

matchCheckFail:
    
    return matchCheckResult;
}

/**
 * packet token과 config token을 match method에 맞는 경우로 서로 비교
 *
 * @param   packetToken     http request packet
 * @param   configToken     config match string
 * @param   matchMethod     any or start or end
 * @return  int             성공시 SUCCESS(0) 실패시 FAIL(-1) 반환
 *
 * */
int matchStringCheck(const token *packetToken, const token *configToken, const token *matchMethod)
{
    int matchCheckResult = 0;
    char packetTokenText = 0;
    char configTokenText = 0;
    unsigned int index = 0;
    unsigned int loopLimit = 0;
    unsigned int loopCount = 0;
    unsigned int packetTokenLastIndex = 0;
    unsigned int configTokenLastIndex = 0;

    /* param excpetion */
    PARAM_EXP_CHECK(packetToken, FAIL);
    PARAM_EXP_CHECK(configToken, FAIL);
    PARAM_EXP_CHECK(matchMethod, FAIL);

#ifdef DEBUG_ROUND_ROBIN_C
    printf("### [DEBUG] --- packet, config token compare start ---\n");
#endif

    /* length check */
    if (configToken->size > packetToken->size) { // config의 비교할 문자열값이 packet(host 나 path)의 토큰 길이 보다 길면 실패
        return FAIL;
    }

    /* match check */
    if (compareTokenAndString(matchMethod, "any", false) == IS_SAME) { // any 일경우
#ifdef DEBUG_ROUND_ROBIN_C
            printf("### [DEBUG] - current match method : any\n");
            printf("packet token : [");
            viewToken(packetToken);
            printf("] configToken : [");
            viewToken(configToken);
            printf("]\n");
#endif           
        loopLimit = ((packetToken->size - configToken->size) + 1);
        for (loopCount = 0; loopCount < loopLimit; loopCount++) { // packet 토큰의 시작지점을 한칸식 옮기면서
            matchCheckResult = SUCCESS; 
            for(index = 0; index < configToken->size; index++) { // config 토큰과 비교한다.
                packetTokenText = packetToken->value[index + loopCount];
                configTokenText = configToken->value[index];
                if (packetTokenText != configTokenText) { // 한문장이라도 다르면 실패
                    matchCheckResult = FAIL;
                    break;
                }

            }
            if (matchCheckResult == SUCCESS) {
                break;
            }
        }
    }
    else if (compareTokenAndString(matchMethod, "start", false) == IS_SAME) { // start 일 경우
#ifdef DEBUG_ROUND_ROBIN_C
            printf("### [DEBUG] - current match method : start\n");
#endif           
        matchCheckResult = SUCCESS;
        for (index = 0; index < configToken->size; index++) { // 0번 인덱스부터 두개의 토큰을 비교한다.
            packetTokenText = packetToken->value[index];
            configTokenText = configToken->value[index];
            if (packetTokenText != configTokenText) {
                matchCheckResult = FAIL;
                break;
            }
        }
    }   
    else if (compareTokenAndString(matchMethod, "end", false) == IS_SAME) {
#ifdef DEBUG_ROUND_ROBIN_C
            printf("### [DEBUG] - current match method : end\n");
#endif           
        matchCheckResult = SUCCESS;
        loopLimit = (configToken->size);
        packetTokenLastIndex = (packetToken->size - 1);
        configTokenLastIndex = (configToken->size - 1);
        for (loopCount = 0; loopCount < loopLimit; loopCount++) { // 서로의 마지막 인덱스 부터 configToken 크기만큼 뒤로가면서 토큰을 비교한다.
            packetTokenText = packetToken->value[packetTokenLastIndex - loopCount];
            configTokenText = configToken->value[configTokenLastIndex - loopCount];
            if (packetTokenText != configTokenText) {
                matchCheckResult = FAIL;
                break;
            }
        }
    }
    else {
        // config 의 match method가 any, srat, end가 아니라면 
        printf("[Exception] match type is only host or path! (current :");
        viewToken(matchMethod);
        printf(")\n");
        matchCheckResult = FAIL;
        goto matchCheckFail;
    }

#ifdef DEBUG_ROUND_ROBIN_C
    printf("### [DEBUG] --- packet, config token compare end ---\n");
#endif

matchCheckFail:

    return matchCheckResult;
}


/**
 * 매개변수로 넘어온 header list 목록에서 name이 HOST인 값 검색
 * HOST가 있다면 해당 HOST의 value token의 주소값을 반환
 *
 * @param   packetHeader    header list
 * @return  token           header host의 value 주소값 반환
 *                          없다면 NULL 반환
 * */
token *searchHeaderHost(const r_list *packetHeader)
{
    int searchResult = 0;
    r_node *currentHeaderNode = NULL;

    /* param exception */
    PARAM_EXP_CHECK(packetHeader, NULL);
    
    if (packetHeader->listCount == 0) {
#ifdef DEBUG_ROUND_ROBIN_C
        printf("packetHeader list is empty!\n");
#endif
        return NULL;
    }

#ifdef DEBUG_ROUND_ROBIN_C
    printf("### [DEBUG] --- search header host start ---\n");
#endif

    /* search header host */
    searchResult = FAIL;
    currentHeaderNode = packetHeader->head;
    while (currentHeaderNode != NULL) { // 현재 header list에 저장된 노드들을 체크한다.
#ifdef DEBUG_ROUND_ROBIN_C
        printf("header name token : [");
        viewToken(&currentHeaderNode->data.name);
        printf("] header value token : [");
        viewToken(&currentHeaderNode->data.value);
        printf("]\n");
#endif

        if (compareTokenAndString(&currentHeaderNode->data.name, "HOST", false) == IS_SAME) { // 만약 header의 name 이 host와 같다면
            searchResult = SUCCESS; // 그만 탐색
            break;
        }
        currentHeaderNode = currentHeaderNode->next;
    }

#ifdef DEBUG_ROUND_ROBIN_C
    printf("### [DEBUG] --- search header host end ---\n");
#endif

    /* return value token */
    if (searchResult == SUCCESS) { // 찾았다면
        return &currentHeaderNode->data.value; // 찾은 host 요소의 value 토큰의 조소값 반환
    }
    else {
        return NULL;
    }
}
