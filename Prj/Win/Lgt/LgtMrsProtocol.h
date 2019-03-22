#ifndef LGT_MRS_PROTOCOL_H
#define LGT_MRS_PROTOCOL_H
#include "LgtType.h"
typedef struct {
	uint8_t ctrl;
	uint8_t commFlag;
	uint8_t id;			//���к�
	uint8_t afn;		//AFN������
	uint8_t dt;			//��Ϣ��DT		
	uint8_t dataLen;	//Ӧ�����ݳ���
	uint8_t data[255];	//Ӧ������
	uint8_t src[6];		//Դ��ַ����
	uint8_t dest[6];	//Ŀ�ĵ�ַ����
} LgtMrsStruct;

int LgtMrsPack(uint8_t *data, LgtMrsStruct *lgtMrsStr);
int LgtMrsUnpack(uint8_t *data, uint16_t len, LgtMrsStruct *lgtMrsStr);
#endif
