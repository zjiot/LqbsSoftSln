//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : arch.h
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : ����ϵͳ�ӿڲ���
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************
#ifndef _ARCH_H_
#define _ARCH_H_
#pragma once

#include <afxwin.h>
#include<windows.h>
#include<WS2tcpip.h>

//#define DTRACE(x)	do{	TRACE(x); REMOTETRACE(x); }while(0)
//#define DTRACE   TRACE

typedef int TTaskRet;
typedef int TPrio;

//PLC�����ں�
#define CCT_PLC_COMM			1
#define CCT_FILE_PATH			"d:\\CctFiles\\"

//��Ϣ������������Ҳ��������Ϣ�����ķְ�����
//�ְ�����Էֱ�ŵ��ĸ������ϣ���һ�㶼������
#define SYS_MSG_PARAM_NUM_MAX	4

#define SOCKET_ERR_SUCCESS		0
#define SOCKET_ERR_FAILED		(-1)

#pragma pack(1)  

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
	unsigned int dwMsgId;
	unsigned long long dwParam[SYS_MSG_PARAM_NUM_MAX];
}CCT_QUEUE_MSG_S;

typedef struct
{
	unsigned int dwFrmSize;
	unsigned char *pFrmData;	//��Ϣ����
}CCT_MSG_ONE_FRM_S;

typedef struct
{
	unsigned short wYear;
	unsigned char bMonth;
	unsigned char bDay;
	unsigned char bHour;
	unsigned char bMinute;
	unsigned char bSecond;
}TTime;

#pragma pack()  

void NewTask(TTaskRet(*pFunc)(void *arg), void *pvArg, int nStackSize, TPrio prio);
unsigned int NewTaskEX(TTaskRet(*pFunc)(void *arg), void *pvArg, int nStackSize, TPrio prio);

signed int NewQueue(unsigned short wQueue, char *pszName, short dwMsgArrayCnt);
signed int SendMessage2Queue(int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, int dwTimeOut);
signed int WaitMessage(int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, unsigned int dwTimeOut);
signed int OpenQueue(char *pszName, unsigned int dwAccess, short dwMsgArrayCnt);
//const char *GetQueueName(signed int dwQueueId);

signed int NewMutex(HANDLE pMutex);
signed int GetMutex(HANDLE pMutex);
signed int RelMutex(HANDLE pMutex);

void GetTime(TTime* pTime);
unsigned int GetClick();
unsigned int GetTick();
signed int OpenComm(unsigned short wPort, unsigned int dwBaudRate, unsigned char bByteSize, unsigned char bStopBits, unsigned char bParity);
unsigned int CommReceive(int hHanlde, unsigned char *pBuf, unsigned short wBufSize, int *error);
bool CommSend(int hHanlde, unsigned char *pBuf, unsigned short wBufSize);
bool CloseComm(int hHandle);

signed int CloseSocket(int *s);
void SetSocketNoBlock(SOCKET Socket);

unsigned long ConvertAddr2IP(const char *pAddr);
signed int SocketShutDown(SOCKET *s);
void AppUpGrade();
void SystemReboot();

#endif