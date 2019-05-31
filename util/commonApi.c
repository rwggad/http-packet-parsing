#include "commonApi.h"

/**
 * 문자열을 ipv4 또는 ipv6 형식에 맞게 구분해서 정수형으로 반환
 *
 * @param   string          문자열
 * @param   addr            ip 정보를 담고 있는 구조체 
 * @return  int             성공 여부 반환 (SUCCESS (0), FAIL (-1)) 
 *
 * */
int str_inet_pton(char *string, struct sockaddr_in *addr)
{
    int inetResult = 0;
    int transResult = 0;

    /* param exception */
    PARAM_EXP_CHECK(string, FAIL);
    PARAM_EXP_CHECK(addr, FAIL);
    
    /* ipS 문자열을 4바이트 정수로 변환 */
    transResult = SUCCESS;
    if (strlen(string) > INET_ADDRSTRLEN) { // ipv6
        addr->sin_family = AF_INET6;
    }
    else { // ipv4
        addr->sin_family = AF_INET;
    }
    inetResult = inet_pton(addr->sin_family, string, &addr->sin_addr.s_addr);
    if (inetResult <= 0) {
        if (inetResult == 0) {
            printf("[Exception] Not in presentation format. (str_inet_pton)\n");
            transResult = FAIL;
        }
        else {
            printf("[Exception] inet_pton errer. (str_inet_pton)\n");
            transResult = FAIL;
        }
    }

    return transResult;
}
/**
 * 메모리의 start ~ end 부분을 ipv4 또는 ipv6 형식에 맞게 구분해서 정수형으로 반환
 *
 * @param   start           메모리 시작 지점
 * @param   end             메모리 끝 지점
 * @param   addr            ip 정보를 담고 있는 구조체 
 * @return  int             성공 여부 반환 (SUCCESS (0), FAIL (-1)) 
 *
 * */
int mem_inet_pton(char *start, char *end, struct sockaddr_in *addr)
{
    int inetResult = 0;
    int transResult = 0;
    char *ipString = NULL;
    unsigned int memSize = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    PARAM_EXP_CHECK(addr, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);
    
    /* 현재 메모리 공간의 문자를 문자열로 변환 */
    memSize = ((end - start) + 1); 
    ipString = (char*)calloc(1, sizeof(char) * (memSize + 1)); // '\0' 처리를 위해 +1
    if (ipString == NULL) {
        printf("[Exception] allocation fail. (mem_inet_pton, ipString)\n");
        return FAIL;
    }

    memcpy(ipString, start, memSize);
    ipString[memSize] = '\0'; // 문자열 처리

    /* ipS 문자열을 4바이트 정수로 변환 */
    transResult = SUCCESS;
    if (strlen(ipString) > INET_ADDRSTRLEN) { // ipv6
        addr->sin_family = AF_INET6;
    }
    else { // ipv4
        addr->sin_family = AF_INET;
    }
    inetResult = inet_pton(addr->sin_family, ipString, &addr->sin_addr.s_addr);
    if (inetResult <= 0) {
        if (inetResult == 0) {
            printf("[Exception] Not in presentation format. (mem_inet_pton)\n");
            transResult = FAIL;
        }
        else {
            printf("[Exception] inet_pton errer. (mem_inet_pton)\n");
            transResult = FAIL;
        }
    }

    /* clean up */
    if (ipString != NULL) {
        free(ipString);
        ipString = NULL;
    }
    return transResult;
}


/**
 * 현재 메모리 블록에 있는 범위 만큼 값과 스트링이랑 비교
 *
 * @param   start   메모리 블록의 시작 위치
 * @param   end     메모리 블록의 마지막 위치
 * @return  Int     변환된 정수형 값 (실패시 : FAIL(-1) 반환)
 *
 * */
int mem_str_i_cmp(const char *start, const char *end, const char *string)
{
    int result = 0;
    char memText = 0;
    char strText = 0;
    unsigned int index = 0;
    unsigned int memSize = 0;
    unsigned int strLength = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);
    PARAM_EXP_CHECK(string, FAIL);

    /* set Size */
    strLength = strlen(string);
    memSize = ((end - start) + 1);

    /* compare */
    if (strLength != memSize) {
        return IS_DIFF; 
    }

    result = IS_SAME;
    for (index = 0; index < strLength; index++) { // 한 인덱스씩 확인하면서 같은지 체크
        memText = (*(start + index));
        strText = string[index];
       
        // 대소문자 구별하지 않음 
        memText = tolower(memText);
        strText = tolower(strText);

        if (memText != strText) {
            result = IS_DIFF;
            break;
        }
    }   
    return result;
}

/**
 * 현재 메모리 블록에 있는 범위 만큼의 값을 숫자로 변환
 *
 * @param   start   메모리 블록의 시작 위치
 * @param   end     메모리 블록의 마지막 위치
 * @return  Int     변환된 정수형 값 (실패시 : FAIL(-1) 반환)
 *
 * */
int mem_atoi(const char *start, const char *end)
{
    int result = 0;
    char *string = NULL;
    unsigned int memSize = 0;

    /* param exception */
    PARAM_EXP_CHECK(start, FAIL);
    PARAM_EXP_CHECK(end, FAIL);
    RANGE_EXP_CHECK(start, end, FAIL);
    
    /* 현재 메모리 공간의 문자를 문자열로 변환 */
    memSize = ((end - start) + 1); // 
    string = (char*)calloc(1, sizeof(char) * (memSize + 1)); // '\0' 처리를 위해 +1
    if (string == NULL) {
        printf("[Exception] allocation fail. (memAtoi function, string)\n");
        return FAIL;
    }

    memcpy(string, start, memSize);
    string[memSize] = '\0'; // 문자열 처리

    /* 문자열을 숫자로 변환 */
    result = atoi(string);
    
    /* clean up */
    if (string != NULL) {
        free(string);
        string = NULL;
    }
    return result;
}

/**
 * 해당 문자열에 들어온 값을 숫자로 변환 해줌 
 *
 * @param   strPort     숫자로 변환 시킬 문자열
 * @param   port        변환된 값을 저장할 변수
 * @return  int         성공 여부 반환 ( SUCCESS (0), FAIL (-1) )
 * */
int port_strtoul(const char *strPort, unsigned short *port)
{
    char *endPtr = NULL;
    unsigned long portNumber = 0;

    /* pararm exception */
    PARAM_EXP_CHECK(strPort, FAIL);
    PARAM_EXP_CHECK(port, FAIL);
    
    if (strlen(strPort) <= 0){
        printf("[Exception] port number is essential element.\n");
        return FAIL;
    }

    /* str to short */
    portNumber = strtoul(strPort, &endPtr, 10);
    if (strlen(endPtr) > 0) {
        printf("[Exception] invalid port. (%s)\n", strPort);
        return FAIL;
    }

    /* port number invalid check */
    if (portNumber > MAX_PORT_NUMBER_RANGE) {
        printf("[Exception] port number range is (%d~%d) current (%lu)\n", MIN_PORT_NUMBER_RANGE, MAX_PORT_NUMBER_RANGE, portNumber);
        return FAIL;
    }

    /* set port number */
    (*port) = (unsigned short) portNumber;

    return SUCCESS;
}
