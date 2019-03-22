#include "LgtMrsProtocol.h"
#include "LgtPublic.h"
#define LGT_MRS_FRAME_START		0x68
#define LGT_MRS_FRAME_END		0x16

int LgtMrsPack(uint8_t *data, LgtMrsStruct *lgtMrsStr)
{
	uint8_t *p = data;
	uint16_t len = 15 + lgtMrsStr->dataLen + (lgtMrsStr->commFlag ? 12 : 0);
	*p++ = LGT_MRS_FRAME_START;
	*p++ = (uint8_t)(len & 0x0f);
	*p++ = (uint8_t)(len >> 8);
	*p++ = lgtMrsStr->ctrl;
	*p++ = lgtMrsStr->commFlag ? 0x04 : 0x00;
	memset(p, 0, 4);
	p += 4;
	*p++ = lgtMrsStr->id;
	if (lgtMrsStr->commFlag) 
	{
		memcpy(p, lgtMrsStr->src, 6);
		p += 6;
		memcpy(p, lgtMrsStr->dest, 6);
		p += 6;
	}
	*p++ = lgtMrsStr->afn;
	*p++ = 1 << ((lgtMrsStr->dt - 1) % 8);
	*p++ = (lgtMrsStr->dt - 1) / 8;
	if (lgtMrsStr->dataLen > 0) 
	{
		memcpy(p, lgtMrsStr->data, lgtMrsStr->dataLen);
		p += lgtMrsStr->dataLen;
	}
	*p++ = LgtSum(&data[3], len - 5);
	*p++ = LGT_MRS_FRAME_END;
	return (p - data);
}

int LgtMrsUnpack(uint8_t *data, uint16_t len, LgtMrsStruct *lgtMrsStr)
{
	uint8_t cs, offset = 0, i, *p;
	uint8_t dtMatrix[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	//判断指针是否为空
	if (data == NULL || lgtMrsStr == NULL)
		return -1;
	//判断帧起始符和帧结束符是否匹配
	if ((data[0] != LGT_MRS_FRAME_START) || (data[len - 1] != LGT_MRS_FRAME_END) || (len < 15))
		return -2;
	cs = LgtSum(&data[3], len - 5);
	//校验码是否正确
	if (cs != data[len - 2])
		return -3;
	lgtMrsStr->ctrl = data[3];
	lgtMrsStr->id = data[9];
	lgtMrsStr->dataLen = len - 15;
	p = &data[10];
	if (data[4] & 0x04) {
		lgtMrsStr->commFlag = 1;
		lgtMrsStr->dataLen - 12;
		memcpy(lgtMrsStr->src, p, 6);
		p += 6;
		memcpy(lgtMrsStr->dest, p, 6);
		p += 6;
	}
	else
	{
		lgtMrsStr->commFlag = 0;
	}
	lgtMrsStr->afn = *p++;
	for (i = 0; i < 8; i++)
	{
		if (dtMatrix[i] == *p)
		{
			break;
		}
	}
	//DI不正确
	if (i >= 8)
	{
		return -4;
	}
	lgtMrsStr->dt = i + 1;
	p += 1;
	lgtMrsStr->dt += (*p++ * 8);
	if (lgtMrsStr->dataLen > 0)
	{
		memcpy(lgtMrsStr->data, p, lgtMrsStr->dataLen);
	}
	return 0;
}