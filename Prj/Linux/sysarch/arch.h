//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : arch.h
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 操作系统差异接口
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************


#ifndef __ARCH_H__
#define __ARCH_H__

#pragma once

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <pthread.h>
#include "stdio.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

//PLC抄表串口号
#define CCT_PLC_COMM			1
//数据库文件保存位置
#define CCT_FILE_PATH			"/usr/bin/"

#define SOCKET_ERR_SUCCESS		0
#define SOCKET_ERR_FAILED		(-1)
#define THREAD_PRIORITY_NORMAL	0
#define SYS_QUEUE_ID_BASE		0x4000
#define MAX_MSG_LEN				128
#define SYS_MSG_PARAM_NUM_MAX	4


//串口参数
#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define ONESTOPBIT          0
#define ONE5STOPBITS        1
#define TWOSTOPBITS         2
#define CBR_110             110
#define CBR_300             300
#define CBR_600             600
#define CBR_1200            1200
#define CBR_2400            2400
#define CBR_4800            4800
#define CBR_9600            9600
#define CBR_14400           14400
#define CBR_19200           19200
#define CBR_38400           38400
#define CBR_56000           56000
#define CBR_57600           57600
#define CBR_115200          115200
#define CBR_128000          128000
#define CBR_256000          256000

typedef void* (*TFunc)(void *);

#pragma pack(1)  

typedef struct 
{
	long type;
	char bData[128];
}SYS_MSG_BUF;


#define TRACE	printf
//#define DTRACE   REMOTETRACE

typedef signed int TTaskRet;
typedef signed int TPrio;
typedef signed int SOCKET;

typedef struct 
{
	unsigned short wPort;
	unsigned int dwBaudRate;
	unsigned char bByteSize;
	unsigned char bStopBits;
	unsigned char bParity;
}TCommPara;

typedef struct
{
	unsigned short wYear;
	unsigned char bMonth;
	unsigned char bDay;
	unsigned char bHour;
	unsigned char bMinute;
	unsigned char bSecond;
}TTime;

typedef struct
{
	signed int dwMsgId;
	unsigned long long dwParam[SYS_MSG_PARAM_NUM_MAX];

}CCT_QUEUE_MSG_S;


typedef struct
{
	unsigned int dwFrmSize;
	unsigned char *pFrmData;	//消息净荷

}CCT_MSG_ONE_FRM_S;

#pragma pack()  


void NewTask(TTaskRet (* function)(void *arg), void *arg, unsigned int nStackSize, TPrio prio);

bool CreateMsgQueueueFile(const char *pszName);
signed int ClearQueue(signed int dwFtokHandle);
signed int NewQueue(unsigned int dwQueueId, char *pszName, unsigned short dwMsgArrayCnt);
signed int WaitMessage(unsigned int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, unsigned int dwTimeOut);
signed int SendMessage2Queue(unsigned int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, unsigned int dwTimeOut);

signed int NewMutex(pthread_mutex_t *pMutex);
signed int GetMutex(pthread_mutex_t *pMutex);
signed int DelMutex(pthread_mutex_t *pMutex);
signed int GetMutexTry(pthread_mutex_t *pMutex);
signed int RelMutex(pthread_mutex_t *pMutex);

unsigned int GetClick();
unsigned int GetTick();

void GetTime(TTime *pTime);

signed int OpenComm(unsigned short wPort, unsigned int dwBaudRate, unsigned char bByteSize, unsigned char bStopBits, unsigned char bParity);
bool SetCommPara(unsigned int fd, unsigned int bps, unsigned int databits, unsigned int stopbits, unsigned int parity);
unsigned int CommReceive(signed int hHanlde, unsigned char *pBuf, unsigned short wBufSize, int *error);
bool CommSend(signed int hHanlde, unsigned char *pBuf, unsigned short wBufSize);
bool CloseComm(signed int hHandle);

signed int CloseSocket(signed int *s);
signed int SocketShutDown(SOCKET *s);
unsigned long ConvertAddr2IP(const char *pAddr);
void SetSocketNoBlock(SOCKET Socket);


int sprintf_s(char *buffer, const char *format, char * src, ...);
void Sleep(unsigned int dwMs);
void AppUpGrade();
void SystemReboot();


#endif
