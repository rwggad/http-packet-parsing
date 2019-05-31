#include "packet_service.h"

/**
 * packet과 config를 파싱하고 매칭된 서버 출력 함수
 *
 * @param   void    매개변수 없음
 * @return  void    반환 값 없음
 * */
void service(void)
{
    request request;

    configTable configTable;
    configDataSet *configData;

    tuple5 tuple5;
    session newSession;
    session *curSession = NULL;
    sessionTable sessionTable;

    token packetTokens;
    token configTokens;

    char packetFilePath[MAX_FILE_PATH_LENGTH];
    char exitCheck = 0;

#ifdef DEBUG_PACKET_SERVICE_C
    printf("### [DEBUG] --- round robin start ---\n");
#endif

    /* init */
    initToken(&configTokens);
	initConfig(&configTable);
    initSessionTable(&sessionTable);

    /* read config */
    if (fileRead(&configTokens, CONFIG_FILE_PATH) == FAIL) {
        goto configServiceFail;
    }
    /* config parsing */
    if(configParsing(&configTokens, &configTable) == FAIL) {
        printf("parsing fail...\n");
        goto configServiceFail;
    }

#ifdef DEBUG_PACKET_SERVICE_C
    viewConfigList(&configTable.config);
#endif

    while (1) {
        /* init */
        initRequest(&request);
        initTuple5(&tuple5);
        initSession(&newSession);
        initToken(&packetTokens);
        memset(packetFilePath, 0, sizeof(packetFilePath));

        /* file Path input */
        printf("input the packet file path..(stop : \"exit\") --> ");
        scanf("%s", packetFilePath);
        clearInputBuffer();

        /* exit check */
        if (strcmp(packetFilePath, "exit") == IS_SAME) {
            printf("really exit..?(y) --> ");
            exitCheck = fgetc(stdin);
            clearInputBuffer();
            if (exitCheck == 'y' || exitCheck == 'Y') {
                break;
            }
        }

        /* read packet */
        if (fileRead(&packetTokens, packetFilePath) == FAIL) {
            printf("input try again packet file path..\n");
            goto packetServiceFail;
        }

        /* packet header 값 파싱 */
        if (packetHeaderParsing(packetFilePath, &tuple5) == FAIL) {
            printf("current packet header parsing fail..\n");
            goto packetServiceFail;
        }

        /* tuple5 값이 세션에 존재하는지 확인 */
        curSession = isExistSession(&sessionTable, &tuple5);
        if (curSession == NULL) { // 존재 하지 않는 다면
            printf("세션 없음\n");

            /* packet parsing */
            if (requestParsing(&packetTokens, &request) == FAIL) {
                printf("current packet elements parsing fail..\n");
                goto packetServiceFail;
            }

#ifdef DEBUG_PACKET_SERVICE_C
            viewRequest(&request);
#endif
            /* match check*/
            configData = matchCheck(&request, &configTable);
            if (configData == NULL) {
                // todo. 매칭이 안됐을때 처리
                printf("not matching current packet.\n");
                goto packetServiceFail;
            }

            /* tuple5 값 new session에 복사 */
            memcpy(&newSession.tuple5, &tuple5, sizeof(struct tuple5));

            /* 현재 config의 서버 정보를 new session에 저장 */
            memcpy(&newSession.sessionServer,
                    &configData->serverList[configData->runTimeIndex].addr, sizeof(struct sockaddr_in));

            /* new Session 을 session Table 에 저장 */
            pushBackSessionList(&sessionTable.sessionList, &newSession);

            /* 현재 config에 있는 서버 목록 round robin*/
            serverRoundRobin(configData);

            /* 현재 새롭게 추가된 세션을 실행 시키기 위해 */
            curSession = &newSession;

#ifdef DEBUG_PACKET_SERVICE_C
            printf("current session list size : %d\n", sessionTable.sessionList.listCount);
#endif
        }

        // 세션 실행 
        runSession(curSession);

packetServiceFail:

        /* clean up */
        deleteTokenValue(&packetTokens);
        deleteRequest(&request);
    }

#ifdef DEBUG_PACKET_SERVICE_C
    printf("### [DEBUG] --- round robin end ---\n");
#endif

configServiceFail:

    /* clean up */
    deleteSessionTable(&sessionTable);
    deleteTokenValue(&configTokens);
    deleteConfig(&configTable);

    return;
}

/**
 * 매개변수로 넘어온 config 에 저장된 서버를 라운드 로빈 해주는 함수
 *
 * @param   configData      서버의 정보가 입력되어 있는 설정 파일
 * @return  void            반환 값 없음
 * */
void serverRoundRobin(configDataSet *configData)
{
    unsigned int serverListSize = 0;
    unsigned int serverCurRunTimeIndex = 0;

    /* param excpetion */
    PARAM_EXP_CHECK_NO_RETURN(configData);

    /* round robin */
    serverListSize = configData->serverListSize; // 현재 config 파일에 저장된 서버 목록의 크기
    serverCurRunTimeIndex = configData->runTimeIndex; // 현재 실행 되었던 서버 리스트의 인덱스
    if (serverCurRunTimeIndex >= (serverListSize - 1)) { // 마지막 서버라면 처음으로
        configData->runTimeIndex = 0;
    }
    else {
        configData->runTimeIndex++;
    }

    return;
}

/**
 * 현재 http packet과 config 목록들 중에서 매칭되는 것이 있는지 확인
 *
 * @param   packet           http request packet
 * @param   configTable      config 파일 목록
 * @return  configDataSet    매칭된 config Data 의 정보
 * */
configDataSet *matchCheck(const request *packet, const configTable *configTable)
{
    int result = 0;
    configNode *currentConfigNode = NULL;

    /* param exception */
    PARAM_EXP_CHECK(packet, NULL);
    PARAM_EXP_CHECK(configTable, NULL);


    if (configTable->config.listCount == 0) {
        return NULL;
    }

#ifdef DEBUG_PACKET_SERVICE_C
    printf("### [DEBUG] --- matching check start ---\n");
#endif

    /* match typei (HOST or PATH) */
    result = NOT_MATCHING;
    currentConfigNode = configTable->config.head; // config list의 노드
    while (currentConfigNode != NULL) { // config list 노드를 다 읽을 때 까지 반복
        if (currentConfigNode->data.type == TYPE_PATH) {
#ifdef DEBUG_PACKET_SERVICE_C
            printf("### [DEBUG] - current type : [path]\n");
#endif
            // path에 저장된 토큰과, 비교할 config의 문자열(match string)  그리고 비교 방법을 넘겨주면서 같은지 체크
            result = matchStringCheck(&packet->line.uri.absolutePath,
                                        &currentConfigNode->data.matchingString,
                                        currentConfigNode->data.method);
        }
        else if (currentConfigNode->data.type == TYPE_HOST) {
#ifdef DEBUG_PACKET_SERVICE_C
            printf("### [DEBUG] - current type : [host]\n");
#endif
            if (packet->header.headerElements[HOST] != NULL) {
                // host의 value토큰 값과, 비교할 config 문자열(match string), 그리고 비교 방법을 넘겨주면서 같은지 체크
                result = matchStringCheck(packet->header.headerElements[HOST],
                                            &currentConfigNode->data.matchingString,
                                            currentConfigNode->data.method);
            }
        }
        else {
            printf("current config type is undefined type. (%d)\n", currentConfigNode->data.type);
            break;
        }

        if (result == MATCHING) {
            break;
        }
        currentConfigNode = currentConfigNode->next;
    }


#ifdef DEBUG_PACKET_SERVICE_C
    printf("### [DEBUG] --- matching check end ---\n");
#endif

    if (result == MATCHING) {
        return &currentConfigNode->data;
    }
    else {
        return NULL;
    }
}

/**
 * packet token과 config token을 match method에 맞는 경우로 서로 비교
 *
 * @param   packetToken     http request packet
 * @param   configToken     config match string
 * @param   matchMethod     any or start or end
 * @return  int         매칭 성공시 MATCHING(0)  매칭 실패시 NOT_MATCHING(-2) 반환 예외시 FAIL(-1) 반환
 * */
int matchStringCheck(const token *packetToken, const token *configToken, const matchMethod method)
{
    char *curValue = NULL;
    int result = 0;
    unsigned int checkLimit = 0;
    unsigned int loopCount = 0;
    unsigned int startIndex = 0;

    /* param excpetion */
    PARAM_EXP_CHECK(packetToken, FAIL);
    PARAM_EXP_CHECK(configToken, FAIL);

#ifdef DEBUG_PACKET_SERVICE_C
    printf("### [DEBUG] --- packet, config token compare start ---\n");
    printf("[packet : ");
    viewToken(packetToken);
    printf("] [config : ");
    viewToken(configToken);
    printf("]\n");
#endif

    /* size check */
    if (configToken->size > packetToken->size) { // config의 비교할 문자열값이 packet(host 나 path)의 토큰 길이 보다 길면 실패
        return NOT_MATCHING;
    }

    /* match check */
    result = NOT_MATCHING;
    if (method == METHOD_ANY) { // any 일경우
#ifdef DEBUG_PACKET_SERVICE_C
        printf("### [DEBUG] - current match method : [any]\n");
#endif
        /**
         * abcdefg (size : 7)
         * abc     (size : 3)
         *
         * abcdefg
         * abc
         *  abc
         *   abc
         *    abc
         *     abc
         *
         * */
        checkLimit = ((packetToken->size - configToken->size) + 1);
        curValue = packetToken->value; // packet value 값 지정
        for (loopCount = 0; loopCount < checkLimit; loopCount++) {
            if (memcmp(curValue, configToken->value, configToken->size) == IS_SAME) {
                result = MATCHING;
                break;
            }
            curValue++; // 포인터를 한칸 앞으로 옮겨서 memcmp를 시도한다.
        }
    }
    else if (method == METHOD_START) { // start 일 경우
#ifdef DEBUG_PACKET_SERVICE_C
        printf("### [DEBUG] - current match method : [start]\n");
#endif
        if (memcmp(packetToken->value, configToken->value, configToken->size) == IS_SAME) {
            result = MATCHING;
        }
    }
    else if (method == METHOD_END) { // end 일 경우
#ifdef DEBUG_PACKET_SERVICE_C
        printf("### [DEBUG] - current match method : [end]\n");
#endif
        startIndex = (packetToken->size - configToken->size);
        if (memcmp((packetToken->value + startIndex), configToken->value, configToken->size) == IS_SAME) {
            result = MATCHING;
        }
    }
    else {
        printf("current config match method is undefined type. (%d)\n", method);
    }

#ifdef DEBUG_PACKET_SERVICE_C
    printf("### [DEBUG] --- packet, config token compare end ---\n");
#endif

    return result;
}

/**
 * 세션 테이블에 현재 매개변수로 들어온 tuple5 값이 있는지 찾는 함수
 *
 * @param       sessionTable    세션 테이블
 * @param       tuple5          검색할 packet 정보
 * @return      session         검색후 session이 존재하면session값 전달 없으면 NULL 전달
 *
 * */
session *isExistSession(const sessionTable *sessionTable, tuple5 *tuple5)
{
    session *result = NULL;
    sessionNode *currentSessionNode = NULL;

    /* param excpetion */
    PARAM_EXP_CHECK(sessionTable, NULL);
    PARAM_EXP_CHECK(tuple5, NULL);

#ifdef DEBUG_PACKET_SERVICE_C
    printf("### [DEBUG] --- session exist check start ---\n");
#endif

    /* check sessionTable */
    if (sessionTable->sessionList.listCount == 0) {
        return NULL;
    }

    currentSessionNode = sessionTable->sessionList.head;
    while (currentSessionNode != NULL) {
        if ((currentSessionNode->data.tuple5.src.sin_addr.s_addr == tuple5->src.sin_addr.s_addr) &&
            (currentSessionNode->data.tuple5.dest.sin_addr.s_addr == tuple5->dest.sin_addr.s_addr) &&
            (currentSessionNode->data.tuple5.src.sin_port == tuple5->src.sin_port) &&
            (currentSessionNode->data.tuple5.dest.sin_port == tuple5->dest.sin_port) &&
            (currentSessionNode->data.tuple5.protocol == tuple5->protocol)) {
            result = &currentSessionNode->data;
            break;
        }
        if ((currentSessionNode->data.tuple5.src.sin_addr.s_addr == tuple5->dest.sin_addr.s_addr) &&
            (currentSessionNode->data.tuple5.dest.sin_addr.s_addr ==tuple5->src.sin_addr.s_addr) &&
            (currentSessionNode->data.tuple5.src.sin_port == tuple5->dest.sin_port) &&
            (currentSessionNode->data.tuple5.dest.sin_port == tuple5->src.sin_port) &&
            (currentSessionNode->data.tuple5.protocol == tuple5->protocol)) {
            result = &currentSessionNode->data;
            break;
        }
        currentSessionNode = currentSessionNode->next;
    }

#ifdef DEBUG_PACKET_SERVICE_C
    printf("### [DEBUG] --- session exist check end ---\n");
#endif
    return &currentSessionNode->data;
}

/**
 * 세션 정보 출력 함수
 *
 * @param       session     출력 할 세션 구조체
 * @return      void        반환 값 없음
 * */
void runSession(const session *session)
{
    ipSet *server = NULL;

    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(session);

    /* 세션 정보 출력 */
    if (session->tuple5.src.sin_family == AF_INET6) {
        char srcIpAddr[INET6_ADDRSTRLEN] = {0, };
        inet_ntop(session->tuple5.src.sin_family, &session->tuple5.src.sin_addr.s_addr, srcIpAddr, sizeof(srcIpAddr));
        printf(" -src ipv6 : [%s]:[%u]\n", srcIpAddr, session->tuple5.src.sin_port);
    }
    else {
        char srcIpAddr[INET_ADDRSTRLEN] = {0, };
        inet_ntop(session->tuple5.src.sin_family, &session->tuple5.src.sin_addr.s_addr, srcIpAddr, sizeof(srcIpAddr));
        printf(" -src ipv4 : [%s]:[%u]\n", srcIpAddr, session->tuple5.src.sin_port);
    }
    if (session->tuple5.dest.sin_family == AF_INET6) {
        char destIpAddr[INET6_ADDRSTRLEN] = {0, };
        inet_ntop(session->tuple5.dest.sin_family, &session->tuple5.dest.sin_addr.s_addr, destIpAddr, sizeof(destIpAddr));
        printf(" -dest ipv6 : [%s]:[%u]\n", destIpAddr, session->tuple5.dest.sin_port);
    }
    else {
        char destIpAddr[INET_ADDRSTRLEN] = {0, };
        inet_ntop(session->tuple5.dest.sin_family, &session->tuple5.dest.sin_addr.s_addr, destIpAddr, sizeof(destIpAddr));
        printf(" -dest ipv4 : [%s]:[%u]\n", destIpAddr, session->tuple5.dest.sin_port);
    }

	printf(" -protocol : %d\n", session->tuple5.protocol);

    if (session->sessionServer.sin_family == AF_INET6) {
        char destIpAddr[INET6_ADDRSTRLEN] = {0, };
        inet_ntop(session->sessionServer.sin_family, &session->sessionServer.sin_addr.s_addr, destIpAddr, sizeof(destIpAddr));
        printf(" -session ipv6 : [%s]:[%u]\n", destIpAddr, session->sessionServer.sin_port);
    }
    else {
        char destIpAddr[INET_ADDRSTRLEN] = {0, };
        inet_ntop(session->sessionServer.sin_family, &session->sessionServer.sin_addr.s_addr, destIpAddr, sizeof(destIpAddr));
        printf(" -session ipv4 : [%s]:[%u]\n", destIpAddr, session->sessionServer.sin_port);
    }

    return;
}
