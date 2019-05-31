#include "parsing_config.h"
#define MAX_LIST_NODE_COUNT 512

/**
 * linked List 초기화
 *
 * @param   target      element 를 초기화 시킬 list 구조체의 메모리 주소
 * @return  void        반환 값 없음
*/
void initConfigList(configList *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* set init */
    memset(target, 0, sizeof(struct configTable));

	return;
}


/**
 * configDataSet 초기화
 *
 * @param   configDataSet       초기화할 구조체의 메모리 주소
 * @return  void                반환 값 없음
 * */
void initConfigDataSet(configDataSet *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);
    
    /* reset request elements */
    memset(target, 0, sizeof(struct configDataSet));

    return;
}

/**
 * list의 뒷쪽에 노드 추가
 *
 * @param   list    node를 추가 시킬 list의 메모리 주소
 * @param   data    list에 추가 시킬 노드 구조체의 메모리 주소
 * @return  int     성공 여부 반환 (SUCCESS 1, FAIL -1)
 *
*/
int pushBackConfigNode(configList *list, configDataSet *data)
{
	configNode *newNode = NULL;

	/* param exception */
    PARAM_EXP_CHECK(list, FAIL);
	PARAM_EXP_CHECK(data, FAIL);
    
    /* list에 저장될 수 있는 최대 갯수가 안넘었는지 확인*/
    if (list->listCount >= MAX_LIST_NODE_COUNT) { // 리스트의 최대 저장 갯수보다 커지면..
        printf("[Exception] Cannot insert more nodes. (current : %d, max : %d)\n", 
                list->listCount, MAX_LIST_NODE_COUNT);
        return FAIL;
    }

#ifdef DEBUG_CONFIG_PARSING_LIST_C
    printf("### [DEBUG] --- list node push start --- \n");
#endif

	/* new Node 생성 후 초기화 */
	newNode = (configNode*)calloc(1, sizeof(configNode));
	if (newNode == NULL) {
		printf("[Exception] memory allocation fail. (backPushNode function, newNode)\n");
		return FAIL;
	}

	/* new Node에 매개변수로 넘어온 값 복사 */
    memcpy(&newNode->data, data, sizeof(struct configDataSet));

	/* tail노드의 다음 값에 new Node를 넣고 tail 값 변경 */
	if (list->head == NULL) {
		list->head = newNode;
	}
	else {
        list->tail->next = newNode;
	}
    list->tail = newNode;
	list->listCount++; // 노드 갯수 증가

#ifdef DEBUG_CONFIG_PARSING_LIST_C
    printf("### current list count is : %d \n", list->listCount);
    printf("### [DEBUG] --- list node push end ---\n");
#endif

	return SUCCESS;
}

/**
 * list 출력
 *
 * @param   list    저장된 node 들을 출력 해주고 싶은 list의 메모리 주소
 * @return  void    반환값 없음
*/
void viewConfigList(const configList *list)
{
    unsigned int index = 0;
    unsigned int label = 0;
    configNode *curNode = NULL;
    ipSet *server = NULL;

	/* param exception */
	PARAM_EXP_CHECK_NO_RETURN(list);

	/* list is empty ? */
    if (list->listCount == 0) {
        return;
    }

    /* view List items */
    curNode = list->head;
    while (curNode != NULL) {
        printf("%d) [%d] [%d] [", (label + 1), curNode->data.type, curNode->data.method);
        viewToken(&curNode->data.matchingString);
        printf("] [ ");
        for (index = 0 ; index < curNode->data.serverListSize; index++){
            server = &curNode->data.serverList[index];
            if (server->addr.sin_family == AF_INET6) {
                char srcIpAddr[INET6_ADDRSTRLEN] = {0, };
                inet_ntop(server->addr.sin_family, &server->addr.sin_addr.s_addr, srcIpAddr, sizeof(srcIpAddr));
                printf("\n\t[ipv6 (%u)][%s:%u]\n", server->addr.sin_addr.s_addr, srcIpAddr, server->addr.sin_port);
            } 
            else {
                char srcIpAddr[INET_ADDRSTRLEN] = {0, };
                inet_ntop(server->addr.sin_family, &server->addr.sin_addr.s_addr, srcIpAddr, sizeof(srcIpAddr));
                printf("\n\t[ipv4 (%u)][%s:%u]\n", server->addr.sin_addr.s_addr, srcIpAddr, server->addr.sin_port);
            }
        }
        printf(" ]\n");
        label++;
        curNode = curNode->next;
    }

    return;
}

/**
 * Config의 요소 중 메모리 할당된 것들을 해제 해줌 (token->value)
 * 
 * @param   target      메모리를 해제할 요소들을 담고 있는 구조체
 * @return  void        반환 값 없음
 *
 * */
void deleteConfigDataSet(configDataSet *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* element allocation free */
    deleteTokenValue(&target->matchingString);

#ifdef DEBUG_CONFIG_PARSING_LIST_C
    printf("### [DEBUG] delete nameValueSet struct..\n");
#endif

    return;
}


/**
 * list 삭제
 *
 * @param   target      삭제하고 싶은 list의 메모리 주소
 * @return  void        반환값 없음
*/
void deleteConfigList(configList *target)
{
    configNode *curNode = NULL;
    configNode *nextNode = NULL;

	/* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

	/* list is empty ? */
    if (target->listCount == 0) {
        return;
    }

    /* view List items */
    curNode = target->head;
    while(1) {
        nextNode = curNode->next;
        /* current Node 삭제 */
        deleteConfigDataSet(&curNode->data);
        free(curNode);
        curNode = NULL;
        if (nextNode == NULL) {
            break;
        }
        curNode = nextNode;
    }

#ifdef DEBUG_CONFIG_PARSING_LIST_C
    printf("### [DEBUG] delete list...\n");
#endif
	return;
}
