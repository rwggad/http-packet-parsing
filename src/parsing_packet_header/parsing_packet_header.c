#include "parsing_packet_header.h"

typedef enum packetHeaderPasringOrder{
    SOURCE_IP_ADDRESS = 0,
    DESTINATION_IP_ADDRESS,
    SOURCE_PORT_NUMBER,
    DESTINATION_PORT_NUMBER,
    PROTOCAL,
}packetHeaderParsingOrder;

/**
 * 매개변수로 들어온 path 에 대해서 packet header 정보(5tuple) 만 남기고 파싱 후
 * src ip, dest ip, src port dest prot protocol 로 파싱 하는 함수 
 *
 * @param   path            입력받은 파일 경로 정보 ( packet header 정보 포함 )
 * @param   tuple5    파싱한 정보를 저장할 구조체
 * @return  int             성공 여부 반환 ( SUCCESS(0), FAIL (-1) )
 *
 * */
int packetHeaderParsing(char* path, tuple5 *tuple5)
{
    int result = 0;
    char *packetHeaderToken = NULL;
    char *splitResult = NULL;
    packetHeaderParsingOrder parsingOrder;  
    
    /* param excpetion */
    PARAM_EXP_CHECK(path, FAIL);
    PARAM_EXP_CHECK(tuple5, FAIL);

#ifdef DEBUG_PACKET_HEADER_PARSING_C
    printf("### [DEBUG] --- packet header parsing start ---\n");
#endif
        
    /* 파일경로에서 packet header 정보만 남기고 파싱*/
    packetHeaderToken = strrchr(path, '/');
    if (packetHeaderToken == NULL) {
        printf("[Exception] invalid file path (%s)\n", path);
        return FAIL;
    }
    packetHeaderToken++; // '/' 한칸 이동 
 
#ifdef DEBUG_PACKET_HEADER_PARSING_C
    printf("### [DEBUG] - ipnut packetHeader : %s\n", path);
    printf("### [DEBUG] - packetHeader : %s\n", packetHeaderToken);
#endif   
    
    /* packet header 정보에서 '_' 기준으로 각각의 주소 포트등을 파싱 */
    result = FAIL;
    parsingOrder = SOURCE_IP_ADDRESS;
    splitResult = strtok(packetHeaderToken, "_");
    while (splitResult != NULL) {
        if (strlen(splitResult) <= 0){
            printf("[Exception] packet header elements is essentail element. (%d)\n", parsingOrder);
            break;
        }
#ifdef DEBUG_PACKET_HEADER_PARSING_C
    printf("### [DEBUG] - current : %s\n", splitResult);
#endif   
        if (parsingOrder == SOURCE_IP_ADDRESS){
            if(str_inet_pton(splitResult, &tuple5->src) == FAIL) {
                break;
            }
            parsingOrder = DESTINATION_IP_ADDRESS;
        }
        else if (parsingOrder == DESTINATION_IP_ADDRESS) {
            if(str_inet_pton(splitResult, &tuple5->dest) == FAIL) {
                break;
            }
            parsingOrder = SOURCE_PORT_NUMBER;
        }
        else if (parsingOrder == SOURCE_PORT_NUMBER) {
            if (port_strtoul(splitResult, &tuple5->src.sin_port) == FAIL) {
                break;
            }
            parsingOrder = DESTINATION_PORT_NUMBER;
        }
        else if (parsingOrder == DESTINATION_PORT_NUMBER){
            if (port_strtoul(splitResult, &tuple5->dest.sin_port) == FAIL) {
                break;
            }
            parsingOrder = PROTOCAL;

        }
        else if (parsingOrder == PROTOCAL){
            if (strcasecmp(splitResult, "TCP") == IS_SAME) {
                tuple5->protocol = TCP;
            }
            else if (strcasecmp(splitResult, "UDP") == IS_SAME) {
                tuple5->protocol = UDP;
            }
            else {
                printf("[Exception] invalid protocol. (%s)\n", splitResult);
                break;
            }
            result = SUCCESS;
            break;
        }
        else {
            printf("[Exception] parsing order exception.\n");
            break;
        }
        splitResult = strtok(NULL, "_"); 
    }

    if (result == FAIL) {
        printf("[Exception] insufficient number of elements.(%d)\n", parsingOrder);
    }

#ifdef DEBUG_PACKET_HEADER_PARSING_C
    if (tuple5->src.sin_family == AF_INET6) {
        char srcIpAddr[INET6_ADDRSTRLEN] = {0, };
        inet_ntop(tuple5->src.sin_family, &tuple5->src.sin_addr.s_addr, srcIpAddr, sizeof(srcIpAddr));
        printf(" -src ipv6 : [%s] %u \n", srcIpAddr, tuple5->src.sin_addr.s_addr);
    } 
    else {
        char srcIpAddr[INET_ADDRSTRLEN] = {0, };
        inet_ntop(tuple5->src.sin_family, &tuple5->src.sin_addr.s_addr, srcIpAddr, sizeof(srcIpAddr));
        printf(" -src ipv4 : [%s] %u \n", srcIpAddr, tuple5->src.sin_addr.s_addr);
    } 
    if (tuple5->src.sin_family == AF_INET6) {
        char destIpAddr[INET6_ADDRSTRLEN] = {0, };
        inet_ntop(tuple5->dest.sin_family, &tuple5->dest.sin_addr.s_addr, destIpAddr, sizeof(destIpAddr));
        printf(" -dest ipv6 : [%s] %u \n", destIpAddr, tuple5->dest.sin_addr.s_addr);
    } 
    else {
        char destIpAddr[INET_ADDRSTRLEN] = {0, };
        inet_ntop(tuple5->dest.sin_family, &tuple5->dest.sin_addr.s_addr, destIpAddr, sizeof(destIpAddr));
        printf(" -dest ipv4 : [%s] %u \n", destIpAddr, tuple5->dest.sin_addr.s_addr);
    }

    printf(" -src_port : %u\n", tuple5->src.sin_port);
	printf(" -dest_port : %u\n", tuple5->dest.sin_port);
	printf(" -protocol : %d\n", tuple5->protocol);
	printf("### [DEBUG] --- packet header parsing end ---\n");

#endif

    return result;
}

