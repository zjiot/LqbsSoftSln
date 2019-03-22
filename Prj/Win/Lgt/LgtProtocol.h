#ifndef LGT_PROTOCOL_H
#define LGT_PROTOCOL_H
#include "LgtType.h"
#define FRAME_START				0x68
#define FRAME_END				0x16
#define LGT_MAC_ADDRESS_LEN		6

#define LGT_FUN_LOGIN_SERVER		0x1000
#define LGT_FUN_HEART				0x1001
#define LGT_FUN_SYNC_ARCHIVES		0x1002
#define LGT_FUN_GET_ALL_ARCHIVES	0x1003
#define LGT_FUN_SET_ALL_LAMP		0x1005
#define LGT_FUN_SET_ONE_LAMP		0x1006
#define LGT_FUN_GET_ALL_LAMP		0x1007
typedef struct {
	uint8_t addr[LGT_MAC_ADDRESS_LEN];
	uint8_t ctrl;
	uint16_t fun;
	uint16_t dataLen;
	uint8_t data[255];
} PacketStruct;
uint8_t LgtSum(uint8_t *data, uint16_t len);
int LgtPack(uint8_t *data, PacketStruct *pktStr);
int LgtUnpack(uint8_t *data, uint16_t len, PacketStruct *pktStr);
#endif
