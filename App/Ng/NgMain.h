//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : CctMain.h
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

#ifndef NGMAIN_H
#define NGMAIN_H
#pragma once

#include "Ng.h"
#include "arch.h"
#include "stdio.h"

#include "DeBug.h"
#include "OnTimer.h"
#include "MsgQueue.h"
#include "MonitorTask.h"
#include "SocketIf.h"



#if 0
#include "stdio.h"

#include "Cct.h"
#include "DeBug.h"
#include "OnTimer.h"
#include "MsgQueue.h"
#include "MonitorTask.h"

#include "CctReader.h"
#include "SocketIf.h"

//国网-南网选择
//#define PRODUCT_CFG_PRODUCT_TYPE_MRS	//国网
#define PRODUCT_CFG_PRODUCT_TYPE_CSG	//南网


#define DBG_CCT					1
#define PORT_CCT_PLC			1		//逻辑端口号
#define COMM_CCT_PLC			1		//物理端口号
#define CCT_LOOPBUF_SIZE 		2048	//循环缓冲区的大小
#define APP_CCT_BUF_SIZE 		2048	//个线程收发消息的缓冲区大小

#define INFO_PLC_PARA	   		21   	//载波参数更改
#define INFO_PLC_CLRRT	   		22		//清路由
#define INFO_PLC_STOPRD			23		//停止抄表
#define INFO_PLC_RESUMERD		24		//恢复抄表	
#define INFO_PLC_RDALLRTINFO 	50		//读取所有节点的中继路由信息
#define INFO_PLC_UPDATE_ROUTE   54		//载波路由器升级
#define INFO_PLC_WIRLESS_CHANGE 57		//参数变更

#define	CCT_MTRPROTO_97			1		//97版645
#define	CCT_MTRPROTO_07			2		//07版645


#define READER_NUM				3		//可创建抄表器实例个数

#define CCTPLC_READER			0
#define RS4851_READER			1
#define RS4852_READER			2

#define CCT_ERR_SUCCESS			0

#define PRINT_BUF_SIZE			1024

//线程保活检测间隔
#define TASK_KEEP_ALIVE_CHECK_TIME	10
#define TASK_UART_PULSE_CHECK_TIME	10


typedef struct
{
	unsigned short wMtrNum;
	char szDbName[32];
	
	
}TCctDbPara;


typedef struct
{
	unsigned char bIP[4];
	unsigned int dwPort;
	
	
}TSocketPara;

TTaskRet AppMainTask(void* pvArg);
TTaskRet MsgDrvTask(void *pvArg);
TTaskRet MsgSendTask(void *pvArg);
TTaskRet CctReaderTask(void *pvArg);
TTaskRet CctReciveTask(void *pvArg);
TTaskRet CctDbMgrTask(void *pvArg);
TTaskRet OnTimerTask(void *pvArg);
TTaskRet MonitorTask(void *pvArg);


//主业务处理
void Run(void *pvArg);


void SysInit();
void InitVar();
void InitDb();
void InitDebug();
bool InitCctReader();
void InitTaskMonitor();

void CctMainTaskKeepAlive();
void MsgDrvTaskKeepAlive();
void MsgSendTaskKeepAlive();
#endif
TTaskRet AppMainTask(void* pvArg);


#endif


