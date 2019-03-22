#include "LgtProtocol.h"
#include "LgtPublic.h"

int LgtPack(uint8_t *data, PacketStruct *pktStr)
{
	uint8_t *p = data;
	uint16_t len;
	if (pktStr == NULL)
		return -1;
	if (pktStr->dataLen > 0 && pktStr->data == NULL)
		return -1;
	len = 11 + pktStr->dataLen;
	*p++ = (uint8_t)FRAME_START;
	*p++ = (uint8_t)len;
	*p++ = (uint8_t)(len >> 8);
	*p++ = (uint8_t)pktStr->fun;
	*p++ = (uint8_t)(pktStr->fun >> 8);
	memcpy(p, pktStr->addr, LGT_MAC_ADDRESS_LEN);
	p += 6;
	*p++ = pktStr->ctrl;
	if (pktStr->dataLen > 0) {
		memcpy(p, pktStr->data, pktStr->dataLen);
		p += pktStr->dataLen;
	}
	*p++ = LgtSum(data, len + 1);
	*p++ = (uint8_t)FRAME_END;
	return (p - data);
}

int LgtUnpack(uint8_t *data, uint16_t len, PacketStruct *pktStr)
{
	uint8_t cs;
	uint16_t i, validLen;
	if (len < 14 || pktStr == NULL)					//长度不对、指针类型错误
		return -1;
	if (data[0] != 0x68 || data[len - 1] != 0x16)	//帧起始、帧结束字节不对
		return -2;
	cs = LgtSum(data, len - 2);
	if (cs != data[len - 2])						//校验码错误
		return -3;
	pktStr->fun = (uint16_t)data[3];
	pktStr->fun += ((uint16_t)(data[4]) << 8);
	memcpy(pktStr->addr, &data[5], LGT_MAC_ADDRESS_LEN);
	validLen = len - 14;
	pktStr->dataLen = (uint16_t)validLen;
	pktStr->dataLen += ((uint16_t)(validLen) >> 8);
	if (pktStr->dataLen > 0) {
		memcpy(pktStr->data, &data[12], pktStr->dataLen);
	}
	return 0;
}
