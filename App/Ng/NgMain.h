//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : CctMain.h
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : 
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
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

//����-����ѡ��
//#define PRODUCT_CFG_PRODUCT_TYPE_MRS	//����
#define PRODUCT_CFG_PRODUCT_TYPE_CSG	//����


#define DBG_CCT					1
#define PORT_CCT_PLC			1		//�߼��˿ں�
#define COMM_CCT_PLC			1		//����˿ں�
#define CCT_LOOPBUF_SIZE 		2048	//ѭ���������Ĵ�С
#define APP_CCT_BUF_SIZE 		2048	//���߳��շ���Ϣ�Ļ�������С

#define INFO_PLC_PARA	   		21   	//�ز���������
#define INFO_PLC_CLRRT	   		22		//��·��
#define INFO_PLC_STOPRD			23		//ֹͣ����
#define INFO_PLC_RESUMERD		24		//�ָ�����	
#define INFO_PLC_RDALLRTINFO 	50		//��ȡ���нڵ���м�·����Ϣ
#define INFO_PLC_UPDATE_ROUTE   54		//�ز�·��������
#define INFO_PLC_WIRLESS_CHANGE 57		//�������

#define	CCT_MTRPROTO_97			1		//97��645
#define	CCT_MTRPROTO_07			2		//07��645


#define READER_NUM				3		//�ɴ���������ʵ������

#define CCTPLC_READER			0
#define RS4851_READER			1
#define RS4852_READER			2

#define CCT_ERR_SUCCESS			0

#define PRINT_BUF_SIZE			1024

//�̱߳�������
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


//��ҵ����
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


