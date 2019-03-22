//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : OnTimer.h
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

//��ʱ���ṹ
typedef struct
{
	unsigned char bUse;
	//��ʱ��ID ������ID��
	unsigned short dwID;
	//��ʱ��������ϢID
	unsigned int dwMsgId;
	//��ʱ�����λ:0-�룻1-���ӣ�2-Сʱ;3-��....
	unsigned char bType;		
	//��ʱ�����
	unsigned int dwIntV;
	//��ʱ��ʼʱ��
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
