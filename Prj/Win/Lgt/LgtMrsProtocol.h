#ifndef LGT_MRS_PROTOCOL_H
#define LGT_MRS_PROTOCOL_H
#include "LgtType.h"
typedef struct {
	uint8_t ctrl;
	uint8_t commFlag;
	uint8_t id;			//序列号
	uint8_t afn;		//AFN功能码
	uint8_t dt;			//信息类DT		
	uint8_t dataLen;	//应用数据长度
	uint8_t data[255];	//应用数据
	uint8_t src[6];		//源地址数组
	uint8_t dest[6];	//目的地址数组
} LgtMrsStruct;

int LgtMrsPack(uint8_t *data, LgtMrsStruct *lgtMrsStr);
int LgtMrsUnpack(uint8_t *data, uint16_t len, LgtMrsStruct *lgtMrsStr);
#endif
