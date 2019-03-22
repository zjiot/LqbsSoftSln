#include "Lgt645Protocol.h"
#include "LgtPublic.h"

int Lgt645Pack(uint8_t *data, Lgt645Struct *lgt645Str)
{
	uint8_t *p = data;
	if (data == NULL || lgt645Str == NULL)
		return -1;
	*p++ = LGT_645_FRAME_START;
	memcpy(p, lgt645Str->mac, 6);
	p += 6;
	*p++ = LGT_645_FRAME_START;
	*p++ = lgt645Str->ctrl;
	*p++ = lgt645Str->dataLen;
	memcpy(p, lgt645Str->data, lgt645Str->dataLen);
	p += lgt645Str->dataLen;
	*p++ = LgtSum(data, 10 + lgt645Str->dataLen);
	*p++ = LGT_645_FRAME_END;
	return (p - data);
}

int Lgt645Unpack(uint8_t *data, uint16_t len, Lgt645Struct *lgt645Str)
{
	uint8_t cs;
	//判断传参指针类型是否为空
	if (data == NULL || lgt645Str == NULL)
		return -1;
	//判断帧起始符、帧结束符是否正确
	if ((data[0] != LGT_645_FRAME_START) || (data[7] != LGT_645_FRAME_START) || (data[len - 1] != LGT_645_FRAME_END))
		return -2;
	cs = LgtSum(data, len - 2);
	//判断校验码是否正确
	if (cs != data[len - 2])
		return -3;
	memcpy(lgt645Str->mac, &data[1], 6);
	lgt645Str->ctrl = data[8];
	lgt645Str->dataLen = data[9];
	memcpy(lgt645Str->data, &data[10], lgt645Str->dataLen);
	return 0;
}