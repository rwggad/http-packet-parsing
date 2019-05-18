#include "../../include/parsing_config.h"
#define MAX_LIST_NODE_COUNT 512

/**
 * linked List 초기화
 *
 * @param   target      element 를 초기화 시킬 list 구조체의 메모리 주소
 * @return  void        반환 값 없음
*/
void initConfigLinkedList(c_list *target)
{
    /* param exception */
    if (target == NULL) {
        printf("[Exception] param is null pointer. (initConfigLinkedList function, target)\n");
        return;
    }

    /* set init */
    memset(target, 0, sizeof(struct c_list));

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
    if (target == NULL) {
        printf("[Exception] param is null pointer. (initConfigDatSet function, target)\n");
        return;
    }
    
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
int pushBackConfigNode(c_list *list, configDataSet *data)
{
	c_node *newNode = NULL;

	/* exception */
	if (list == NULL) {
		printf("[Exception] param is null pointer. (backPushNode function, list) \n");
		return FAIL;
    }
    if (data == NULL) {
        printf("[Exception] param is null pointer. (backPushNode function, data) \n");
        return FAIL;
    }

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
	newNode = (c_node*)calloc(1, sizeof(c_node));
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
void viewConfigLinkedList(const c_list *list)
{
    unsigned int label = 0;
    c_node *currentNode = NULL;

	/* param exception */
	if (list == NULL) {
		printf("[Exception] param is null pointer..! (list) \n");
		return;
	}

	/* list is empty ? */
    if (list->listCount == 0) {
        return;
    }

    /* view List items */
    currentNode = list->head;
    while (currentNode != NULL) {
        printf("%d) [", label);
        viewToken(&currentNode->data.matchType);
        printf("] [");
        viewToken(&currentNode->data.matchMethod);
        printf("] [");
        viewToken(&currentNode->data.matchingString);
        printf("] [");
        printf("%d] [%d] ", currentNode->data.runTimeIndex, currentNode->data.serverListSize);
        for (int i = 0 ; i < currentNode->data.serverListSize; i++){
            printf("[");
            viewToken(&currentNode->data.serverList[i]);
            printf("] ");
        }
        printf("\n");
        label++;
        currentNode = currentNode->next;
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
    int arrayIndex = 0;
    /* param exception */
    if (target == NULL) {
        printf("[Exception] param is null pointer. (deleteConfigDataSet function, target)\n");
        return;
    }

    /* element allocation free */
    deleteTokenValue(&target->matchType);
    deleteTokenValue(&target->matchMethod);
    deleteTokenValue(&target->matchingString);
    for (arrayIndex = 0; arrayIndex < target->serverListSize; arrayIndex++) {
        deleteTokenValue(&target->serverList[arrayIndex]);
    }

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
void deleteConfigLinkedList(c_list *target)
{
    c_node *currentNode = NULL;
    c_node *nextNode = NULL;

	/* param exception */
	if (target == NULL) {
		printf("[Exception] param is null pointer. (deleteConfigLinkedList function, target) \n");
		return;
	}

	/* list is empty ? */
    if (target->listCount == 0) {
        return;
    }

    /* view List items */
    currentNode = target->head;
    while(1) {
        nextNode = currentNode->next;
        /* current Node 삭제 */
        deleteConfigDataSet(&currentNode->data);
        free(currentNode);
        currentNode = NULL;
        if (nextNode == NULL) {
            break;
        }
        currentNode = nextNode;
    }

#ifdef DEBUG_CONFIG_PARSING_LIST_C
    printf("### [DEBUG] delete list...\n");
#endif
	return;
}
