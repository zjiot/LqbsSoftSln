#ifndef LGT_TYPE_H
#define LGT_TYPE_H
#include <string>
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char sint8_t;
typedef signed sint16_t;
typedef signed int sint16_t;
#ifndef NULL
#define NULL (void *)0
#endif

typedef struct {
	uint8_t mac[8];
	uint8_t onoff;
	uint8_t bright;
	uint8_t mask;		//用来定时控制路灯
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} LgtArchives;

#endif

