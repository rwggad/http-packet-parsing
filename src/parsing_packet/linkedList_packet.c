#include "../../include/parsing_packet.h"
#define MAX_LIST_NODE_COUNT 512

/**
 * linked List 초기화
 *
 * @param   target  element 를 초기화 시킬 list 구조체의 메모리 주소
 * @return  void    반환 값 없음
*/
void initRequestLinkedList(r_list *target)
{
    /* param exception */
    if (target == NULL) {
        printf("[Exception] param is null pointer ..! (target)\n");
        return;
    }

    /* set init */
    memset(target, 0, sizeof(struct r_list));

	return;
}


/**
 * nameValueSet 초기화
 *
 * @param   target      초기화할 구조체의 메모리 주소
 * @return  void        반환 값 없음
 * */
void initNameValueSet(nameValueSet *target)
{
    if (target == NULL) {
        printf("[Exception] param is null pointer. (initNameValueSet function, target)\n");
        return;
    }
    
    /* reset request elements */
    initToken(&target->name);
    initToken(&target->value);
    
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
int pushBackRequestNode(r_list *list, nameValueSet *data)
{
	r_node *newNode = NULL;

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

#ifdef DEBUG_REQUEST_PARSING_LIST_C
    printf("### [DEBUG] --- list node push start --- \n");
#endif

	/* new Node 생성 후 초기화 */
	newNode = (r_node*)calloc(1, sizeof(struct r_node));
	if (newNode == NULL) {
		printf("[Exception] memory allocation fail. (backPushNode function, newNode)\n");
		return FAIL;
	}

	/* new Node에 매개변수로 넘어온 값 복사 */
    memcpy(&newNode->data, data, sizeof(struct nameValueSet));

	/* tail노드의 다음 값에 new Node를 넣고 tail 값 변경 */
	if (list->head == NULL) {
		list->head = newNode;
	}
	else {
        list->tail->next = newNode;
	}
    list->tail = newNode;
	list->listCount++; // 노드 갯수 증가

#ifdef DEBUG_REQUEST_PARSING_LIST_C
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
void viewRequestLinkedList(const r_list *list)
{
	int labelCount = 0; // label..
    r_node *currentNode = NULL;

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
        printf("\t%d. name : \"", (labelCount + 1));
        viewToken(&currentNode->data.name);
        printf("\", value : \"");
        viewToken(&currentNode->data.value);
        printf("\"\n");
        labelCount++;
        currentNode = currentNode->next;
    }

    return;
}

/**
 * nameValueSet의 요소 중 메모리 할당된 것들을 해제 해줌 (token->value)
 * 
 * @param   target      메모리를 해제할 요소들을 담고 있는 구조체
 * @return  void        반환 값 없음
 *
 * */
void deleteNameValueSet(nameValueSet *target)
{
    /* param exception */
    if (target == NULL) {
        printf("[Exception] param is null pointer. (deleteNameValueSet function, target)\n");
        return;
    }

    /* element allocation free */
    deleteTokenValue(&target->name);
    deleteTokenValue(&target->value);

#ifdef DEBUG_REQUEST_PARSING_LIST_C
    printf("### [DEBUG] delete nameValueSet struct..\n");
#endif

    return;
}


/**
 * list 삭제
 *
 * @param   target      삭제하고 싶은 target의 메모리 주소
 * @return  void        반환값 없음
*/
void deleteRequestLinkedList(r_list *target)
{
    r_node *currentNode = NULL;
    r_node *nextNode = NULL;

	/* param exception */
	if (target == NULL) {
		printf("[Exception] param is null pointer..! (deleteRquestLinkedList function, target) \n");
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
        deleteNameValueSet(&currentNode->data);
        free(currentNode);
        currentNode = NULL;
        if (nextNode == NULL) {
            break;
        }
        currentNode = nextNode;
    }

#ifdef DEBUG_REQUEST_PARSING_LIST_C
    printf("### [DEBUG] delete list...\n");
#endif
	return;
}
