#ifndef __ROUND_ROBIN_H__
#define __ROUND_ROBIN_H__

#include "main.h"
#include "token.h"
#include "file_read.h"
#include "parsing_packet.h"
#include "parsing_packet_header.h"
#include "parsing_config.h"

//#define DEBUG_PACKET_SERVICE_C 
#define CONFIG_FILE_PATH "res/config_file"

// ------ function ------

void service(void);
void serverRoundRobin(configDataSet *configData);

configDataSet *matchCheck(const request *packet, const configTable *configList); 
int matchStringCheck(const token *packetToken, const token *configToken, const matchMethod method);

session *isExistSession(const sessionTable *sessionTable, tuple5 *tuple5);
void runSession(const session *session);

#endif
