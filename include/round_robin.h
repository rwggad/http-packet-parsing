#ifndef __ROUND_ROBIN_H__
#define __ROUND_ROBIN_H__

#include "main.h"
#include "token.h"
#include "parsing_packet.h"
#include "parsing_config.h"

//#define DEBUG_ROUND_ROBIN_C 

// ------ function ------

token *searchHeaderHost(const r_list *packetHeader);  
int matchCheck(const request *packet, const config *configList); 
int matchStringCheck(const token *packetToken, const token *configToken, const token *matchMethod);
#endif
