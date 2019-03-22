//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : MsgQueue.h
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

#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include "arch.h"


//��ϢID-��Ϣ�ĳ����
//�������������� ��Ҫ�࿪MsgDrvTask
//�࿪һ��������������Ҫ����һ���ĸ�������Ϣ
//Ŀǰֻ����һ������������
typedef enum
{
	//����������
	APP_MSG_ID_SVR_REQ = 0,
	APP_MSG_ID_SVR_REP,

	//���ݿ�����
	APP_MSG_ID_DB_REQ,
	APP_MSG_ID_DB_REP,	

	//����·�����󳭶��ڵ�����
	APP_MSG_ID_CCT_REQ,
	APP_MSG_ID_CCT_CON,

	//·���е�������Ϣ
	APP_MSG_ID_CCT_TOP_REQ,
	APP_MSG_ID_SVR_REP_TOP_DATA,
	APP_MSG_ID_TIMER_HEART_BEAT,

	//645�������ݷ���
	APP_MSG_ID_SVR_REP_645_DATA,

	//�¼��ϱ�
	APP_MSG_ID_SVR_REP_EVENT_DATA,

	//��ʱ��ע��/ɾ����Ϣ
	APP_MSG_ID_REG_ONTIMER,
	APP_MSG_ID_DEL_ONTIMER,

	//�붨ʱ����ϢID
	APP_MSG_ID_SEC_ONTIMER,
	APP_MSG_ID_10MS_ONTIMER,

	//��LTEͨ���̶߳��з��𳭶��ܱ�����(ͨ������ն�-376.1Э��)
	APP_MSG_ID_LTE_REQ,
	APP_MSG_ID_LTE_REP,

	APP_MSG_ID_DRV_10MS,
	APP_MSG_ID_CCT_MAINNODE,

	//����MsgIDר�����̼߳��
	/////////////////////////////////
	APP_MSG_ID_CCT_DBMGR_TASK_ALIVE,
	APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE,
	APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE,
	APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE,
	APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE,
	APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE,
	APP_MSG_ID_CCT_MAIN_TASK_ALIVE,

	//�����������
	APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE,
	//APP_MSG_ID_CCT_MONITOR_TASK,
	/////////////////////////////////

	// TODO ... 
	LGT_MSG_ID_LOGIN_RESEND,
	LGT_MSG_ID_HEART,
	APP_MSG_ID_MAX
}APP_MSG_ID_ENUM;


//����ID
//�������������� ��Ҫ�࿪���MsgDrvTask
//ͬʱ����������е�ID��ʶAPP_MAIN_QUEUE_DRV1��2��3....
typedef enum
{
	APP_QUEUE_ID_MAIN = 0,		//���̶߳���
	APP_QUEUE_ID_DRV,			//ͨ���̶߳���
	APP_QUEUE_ID_DRVRCV,		//ͨ���̶߳���-����
	APP_QUEUE_ID_DB,			//���ݿ�����̶߳���
	APP_QUEUE_ID_CCT,			//PLC�����̶߳���
	APP_QUEUE_ID_CCTRCV,		//PLC�������ݽ����̶߳���
	APP_QUEUE_ID_TIM,			//��ʱ���̶߳���
	APP_QUEUE_ID_LTE,			//LTEģ��ͨ���߳�ά������
	APP_QUEUE_ID_MINITOR,		//�̼߳���̶߳���
    // TODO ... 
	
	APP_QUEUE_ID_MAX		//���б�Ž���������ĩβ
} APP_QUEUE_ID_ENUM;


bool InitQueue();
const char *GetQueueName(signed int dwQueueId);
signed int GetQueueHandle(APP_QUEUE_ID_ENUM QueueId);
bool QueueName2Id(const char *pszName, signed int *pdwQueueId);
signed int AppMainMsgProc(unsigned int dwMsgId, unsigned char *pRxBuf, unsigned int dwRxLen, unsigned char *pTxBuf, unsigned int dwTxLen);
signed int AppSendMessage2Queue(APP_QUEUE_ID_ENUM dwQueId, APP_MSG_ID_ENUM dwMsgId, unsigned char *pBuf, unsigned int dwSize);
signed int AppRcvMessage(APP_QUEUE_ID_ENUM dwQueId, APP_MSG_ID_ENUM *pMsgId, unsigned char *pBuf, unsigned int *pSize, unsigned int dwTimeOut);


#endif
