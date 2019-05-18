#include "../include/main.h"
#include "../include/token.h"
#include "../include/file_read.h"
#include "../include/parsing_packet.h"
#include "../include/parsing_config.h"
#include "../include/round_robin.h"

#define CONFIG_FILE_PATH "../resources/config_file"

/**
 * packet과 config를 파싱하고 매칭된 서버 출력 함수
 *
 * @param   void    매개변수 없음
 * @return  void    반환 값 없음
 * */
void roundRobin(void)
{
    config config;
    request request; 
    token packetTokens;
    token configTokens;
    char exitCheck = 0;
    char packetFilePath[MAX_FILE_PATH_LENGTH];

	/* init */
    initToken(&configTokens);
	initConfig(&config);

    /* read config */
    if (fileRead(&configTokens, CONFIG_FILE_PATH) == FAIL) {
        goto configFileReadFail;
    }
    /* config parsing */
    if(configParsing(&configTokens, &config) == FAIL) {
        printf("parsing fail...\n");
        goto configParsingFail;
    }
    
    //viewConfigLinkedList(&config.configDataList);

    while (1) {
        initToken(&packetTokens);
        initRequest(&request);
        /* file Path input */
        printf("input the packet file path..(stop : \"exit\") --> ");
        scanf("%s", packetFilePath);
        clearInputBuffer();
        /* exit check */
        if (strncmp(packetFilePath, "exit", sizeof("exit")) == IS_SAME) {
            printf("really exit..?(y) --> ");
            exitCheck = fgetc(stdin);
            if (exitCheck == 'y' || exitCheck == 'Y') {
                goto packetParsingEnd;
            }
            clearInputBuffer();
        }

        /* read packet */
        if (fileRead(&packetTokens, packetFilePath) == FAIL) {
            printf("input try again packet file path..\n");
            goto packetFileReadFail;
        }
        /* packet parsing */
        if (requestParsing(&packetTokens, &request) == FAIL) {
            printf("parsing fail...\n");
            goto packetParsingFail;
        }
        
        //viewrequest(&request);
        
        /* match check */
        if (matchCheck(&request, &config) == FAIL) {
            printf("재전송..\n");
            /*
             * packet이 들어온 서버로 재 전송.. 
             * */
        }

packetFileReadFail:
packetParsingFail:
packetParsingEnd: 

        /* clean up */
        memset(packetFilePath, 0, sizeof(packetFilePath));
        deleteTokenValue(&packetTokens);
        deleteRequest(&request);
        if(exitCheck == 'y' || exitCheck == 'Y'){
            break;
        }
    }

configFileReadFail:	
configParsingFail:
    
    /* clean up */
    deleteTokenValue(&configTokens);
    deleteConfig(&config);
}

int main(void)
{
    roundRobin();
    return 0;
}


