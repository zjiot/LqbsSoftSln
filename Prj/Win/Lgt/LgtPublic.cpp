#include "LgtPublic.h"

uint8_t LgtSum(uint8_t *data, uint16_t len)
{
	uint16_t i = 0;
	uint8_t sum = 0;
	for (i = 0; i < len; i++)
	{
		sum += data[i];
	}
	return sum;
}