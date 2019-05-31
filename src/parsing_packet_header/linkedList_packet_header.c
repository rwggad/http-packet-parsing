#include "parsing_packet_header.h"
#define MAX_LIST_NODE_COUNT 2

/**
 * sessionTable List 초기화
 *
 * @param   target      element 를 초기화 시킬 list 구조체의 메모리 주소
 * @return  void        반환 값 없음
*/
void initSessionList(sessionList *target)
{
    /* param exception */
    PARAM_EXP_CHECK_NO_RETURN(target);

    /* set init */
    memset(target, 0, sizeof(struct sessionList));

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
int pushBackSessionList(sessionList *list, session *data)
{
	sessionNode *newNode = NULL;

	/* param exception */
    PARAM_EXP_CHECK(list, FAIL);
	PARAM_EXP_CHECK(data, FAIL);
    
    /* list에 저장될 수 있는 최대 갯수가 안넘었는지 확인*/
    if (list->listCount >= MAX_LIST_NODE_COUNT) { // 리스트의 최대 저장 갯수보다 커지면..
#ifdef DEBUG_PACKET_HEADER_PARSING_LIST_C
    printf("### [DEBUG] list size is full.. ! pop front session Node \n");
#endif
        session popSession;
        initSession(&popSession);
        popFrontSessionList(list, &popSession);
    }

#ifdef DEBUG_PACKET_HEADER_PARSING_LIST_C
    printf("### [DEBUG] --- list node push start --- \n");
#endif

	/* new Node 생성 후 초기화 */
	newNode = (sessionNode*)calloc(1, sizeof(sessionNode));
	if (newNode == NULL) {
		printf("[Exception] memory allocation fail. (backPushNode function\n");
		return FAIL;
	}

	/* new Node에 매개변수로 넘어온 값 복사 */
    memcpy(&newNode->data, data, sizeof(struct session));

	/* tail노드의 다음 값에 new Node를 넣고 tail 값 변경 */
	if (list->head == NULL) {
		list->head = newNode;
	}
	else {
        list->tail->next = newNode;
	}
    list->tail = newNode;
	list->listCount++; // 노드 갯수 증가

#ifdef DEBUG_PACKET_HEADER_PARSING_LIST_C
    printf("### current list count is : %d \n", list->listCount);
    printf("### [DEBUG] --- list node push end ---\n");
#endif

	return SUCCESS;
}

/**
 * list의 앞쪽 노드 제거 
 *
 * @param   list        node를 추가 시킬 list의 메모리 주소
 * @param   popSession     pop 한 정보를 담을 구조체 
 * @return  void        반환 값 없음
*/
void popFrontSessionList(sessionList *list, session *popSession)
{
    sessionNode *popSessionNode = NULL;

	/* param exception */
    PARAM_EXP_CHECK_NO_RETURN(list);
    PARAM_EXP_CHECK_NO_RETURN(popSession);
   
    /* list 의 값이 없다면 */
    if (list->listCount == 0) {
        printf("list is empty!\n");
        return;
    }

    /* 앞쪽 노드 제거 */
    popSessionNode = list->head; // pop 하고자 하는 node 정보 저장
    list->head = list->head->next; // list head 를 그 다음 노드로 변경
	list->listCount--; // 노드 갯수 감소

    /* pop 한 노드 정보 저장 및 메모리 할당 해제 */
    memcpy(popSession, &popSessionNode->data, sizeof(struct session));
    free(popSessionNode);
    popSessionNode = NULL;

	return;
}

/**
 * list 삭제
 *
 * @param   target      삭제하고 싶은 list의 메모리 주소
 * @return  void        반환값 없음
*/
void deleteSessionList(sessionList *target)
{
    sessionNode *curNode = NULL;
    sessionNode *nextNode = NULL;

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
        free(curNode);
        curNode = NULL;
        if (nextNode == NULL) {
            break;
        }
        curNode = nextNode;
    }

#ifdef DEBUG_PACKET_HEADER_PARSING_LIST_C
    printf("### [DEBUG] delete list...\n");
#endif
	return;
}
