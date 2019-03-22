//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : OnTimer.h
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************

#ifndef ONTIMER_H
#define ONTIMER_H

#include <list>
#include "arch.h"
#include "MsgQueue.h"

using namespace std;

#define TIME_INTU_10MSEC	0
#define TIME_INTU_SEC		1
#define TIME_INTU_MIN		2
#define TIME_INTU_HOUR		3
#define TIME_INTU_DAY		4
#define TIME_INTU_MON		5

//定时器结构
typedef struct
{
	unsigned char bUse;
	//定时器ID 跟队列ID绑定
	unsigned short dwID;
	//定时器产生消息ID
	unsigned int dwMsgId;
	//定时间隔单位:0-秒；1-分钟；2-小时;3-天....
	unsigned char bType;		
	//定时器间隔
	unsigned int dwIntV;
	//定时起始时间
	unsigned int dwStart;
	
}TTOnTimer;

typedef list<TTOnTimer>			LIST_TIMERINFO;
typedef list<TTOnTimer>::iterator  LIST_TIMERINFO_ITER;

signed int RegisterOnTimer(TTOnTimer *pOnTimer);
signed int DeleteOnTimer(TTOnTimer *pOnTimer);
signed int AddOnTimer(TTOnTimer *ptOnTimer);

void CheckTimeOver();
void OnTimerTaskKeepAlive();

#endif
