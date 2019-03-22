#ifndef LGT_645_PROTOCOL_H
#define LGT_645_PROTOCOL_H
#include "LgtType.h"
#define LGT_645_FRAME_START		0x68
#define LGT_645_FRAME_END		0x16
typedef struct {
	uint8_t mac[6];		//��ַ��
	uint8_t ctrl;		//������
	uint8_t dataLen;		//�����򳤶�
	uint8_t data[255];	//����
}Lgt645Struct;
int Lgt645Pack(uint8_t *data, Lgt645Struct *lgt645Str);
int Lgt645Unpack(uint8_t *data, uint16_t len, Lgt645Struct *lgt645Str);
#endif
