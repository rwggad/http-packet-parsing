#include "token.h"

/**
 * token 초기화
 *
 * @param   token   초기화할 token 구조체의 메모리 주소
 * @return  void    반환 값 없음
*/
void initToken(token *token)
{
    if (token == NULL) {
        printf("[Exception] param is null pointer. (initToken function, token)\n");
        return;
    }

    /* reset token elements */
    memset(token, 0, sizeof(struct token)); // token element 초기화 (value, size)
    
    return;
}

/**
 * token의 요소인 value 에 할당된 메모리 해제
 *
 * @param   token   메모리 해제할 요소를 담고 있는 token 구조체의 메모리 주소
 * @return  void    반환 값 없음
 * */
void deleteTokenValue(token *token)
{
    /* param exception */
    if (token == NULL) {
        printf("[Exception] param is null pointer. (deleteTokenValue function, token)\n");
        return;
    }

#ifdef DEBUG_TOKEN_C
    printf("### [DEBUG] token target : ");
    viewToken(token);
#endif

    /* element allocation free (token->value) */
    if (token->value != NULL) {
        free(token->value);
        token->value = NULL;
    }

#ifdef DEBUG_TOKEN_C
    printf("\n### [DEBUG] delete token..\n");
#endif

    return;
}


/**
 * start ~ end 만큼의 값을 저장
 *
 * @param   token       값을 저장 시킬 token 주소
 * @param   start       현재 저장하려는 문자 배열의 첫번째 주소를 가르키고 있는 포인터
 * @param   end         현재 저장하려는 문자 배열의 마지막 주소를 가르키고 있는 포인터
 * @return  int         저장 성공 여부 반환 (SUCCESS 1, FAIL -1)
 * 
 * */
int saveTokenValue(token *token, char *start, char *end)
{
    int saveResult = 0;
    unsigned int tokenSize = 0;

    /* exception */
    if (token == NULL) {
        printf("[Exception] param is null pointer. (saveTokenValue function, token)\n");
        return FAIL;
    }
    if (start == NULL) {
        printf("[Exception] param is null pointer. (saveTokenValue function start)\n");
        return FAIL;
    }
    if (end == NULL) {
        printf("[Exception] param is null pointer. (saveTokenValue function, end)\n");
        return FAIL;
    }
    if (start > end) {
        printf("[Exception] start pointer must be smaller than end pointer. (saveTokenValue function, start > end) \n");
        return FAIL;
    }
    
    /* allocation and token value set*/
    tokenSize = ((end - start) + 1);
    token->value = (char*)calloc(1, (sizeof(char) * tokenSize));
    if (token->value == NULL) {
        printf("[Exception] allocation fail. (saveTokenValue funciton, token->value)\n");
        return FAIL;
    }
    memcpy(token->value, start, (sizeof(char) * tokenSize));
    token->size = tokenSize;

#ifdef DEBUG_TOKEN_C
    viewToken(token);
    printf("\n");
#endif
    return saveResult;
}

/**
 * token의 값 출력 함수
 *
 * @param   tokens      설정할 토큰 정보가 있는 문자열
 * @return  void        반환 값 없음
 *
 * */
void viewToken(const token *token)
{
    /* declaration and init */
    unsigned int tokenIndex = 0;

    /* param exception */
    if (token == NULL) {
        printf("[Exception] param is null pointer. (view Token function, token) \n");
        return;
    }

    /* token print*/
    if (token->size == 0) { // token is empty
        printf("none");
    }
    else {
        for (tokenIndex = 0; tokenIndex < token->size; tokenIndex++) {
            fputc(token->value[tokenIndex], stdout);
        }
    }
 
#ifdef DEBUG_TOKEN_C
    printf("(%d)", token->size);
#endif

    return;
}
