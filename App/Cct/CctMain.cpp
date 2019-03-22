//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : CctMain.cpp
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
#include <list>
#include "CctMain.h"
#include "DbMgr.h"
#include "CctCfg.h"
#include "DbCfg.h"
#include "HisiReader.h"
#include "CctAppAdapter.h"

TCctPara tCctPara;
CCctReader* g_pCctReader[READER_NUM] = { NULL, NULL, NULL };

TCctDbPara tCctDbPara;
CCctAppAdapter *pAppAdapter = NULL;

TTOnTimer g_tOnTimer[128];





//*****************************************************************************
// ��������: AppMainTask
// ��������: ������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
TTaskRet AppMainTask(void *pvArg)
{
	SysInit();

	//���ݿ����
	NewTask(CctDbMgrTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//ͨ�Ž����߳�
	NewTask(MsgDrvTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//ͨ�ŷ����߳�
	NewTask(MsgSendTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//PLC����
	NewTask(CctReaderTask, NULL, 0, THREAD_PRIORITY_NORMAL);


	NewTask(CctReciveTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//��ʱ������
	NewTask(OnTimerTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//�̼߳��
	NewTask(MonitorTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//Զ�̵��Լ��
	NewTask(ReMoteTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//��ҵ�����߳�
	Run(pvArg);

	return 0;
}


//*****************************************************************************
// ��������: Run
// ��������: ҵ����Ϣ�ַ�
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void Run(void *pvArg)
{
	TRACE("AppCctMainTask::Run().\n");

	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwRxLen = APP_CCT_BUF_SIZE;
	unsigned int dwTxLen = APP_CCT_BUF_SIZE;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return;
	memset(pRxBuf, 0, APP_CCT_BUF_SIZE);

	unsigned char *pTxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pTxBuf == NULL)return;
	memset(pTxBuf, 0, APP_CCT_BUF_SIZE);

	//ע�ᶨʱ��-���ڷ�������֡
	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 280;
	tOnTimer.dwID = APP_QUEUE_ID_MAIN;
	tOnTimer.dwMsgId = APP_MSG_ID_TIMER_HEART_BEAT;
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_MAIN;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_MAIN_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);

	for (;;)
	{
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_MAIN, &dwMsgId, pRxBuf, &dwRxLen, 0xffffffff))
		{
			AppMainMsgProc(dwMsgId, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		}

		//Sleep(10);
	}

	return;
}

//*****************************************************************************
// ��������: CctMainTaskKeepAlive
// ��������: �̷߳�ֹ�����ļ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void CctMainTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_MAIN_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}


//*****************************************************************************
// ��������: AppMainMsgProc
// ��������: ҵ����������Ϣ�ַ�
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed int AppMainMsgProc(unsigned int dwMsgId, unsigned char *pRxBuf, unsigned int dwRxLen, unsigned char *pTxBuf, unsigned int dwTxLen)
{	
	
	if ((NULL == pRxBuf) || (0 == dwRxLen) || (pAppAdapter == NULL))
	{
		return 0;
	}

	switch (dwMsgId)
	{
	case APP_MSG_ID_SVR_REQ:
		pAppAdapter->DispatchPackage(pRxBuf, pTxBuf);
		break;

	case APP_MSG_ID_SVR_REP_645_DATA:
		pAppAdapter->AdapterDoPackage(ADAPTER_CCT_APP_FN4, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		break;

	case APP_MSG_ID_SVR_REP_EVENT_DATA:
		pAppAdapter->AdapterDoPackage(ADAPTER_CCT_APP_FN5, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		break;

	case APP_MSG_ID_SVR_REP_TOP_DATA:
		pAppAdapter->AdapterDoPackage(ADAPTER_CCT_APP_FN6, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		break;
	case APP_MSG_ID_TIMER_HEART_BEAT:
		pAppAdapter->CctAppHeartBeat(ADAPTER_CCT_APP_FN1, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		break;
	case APP_MSG_ID_CCT_MAINNODE:
		pAppAdapter->AdapterDoSaveMainNode(pRxBuf, dwRxLen);
		break;

	case APP_MSG_ID_CCT_MAIN_TASK_ALIVE:
		CctMainTaskKeepAlive();
		break;
		
	default:

		break;
	}

	return 0;

}


#define FSM_CON_STATUS_DISCON			0
#define FSM_CON_STATUS_IF_CON			1
#define FSM_CON_STATUS_TRAN				2

//*****************************************************************************
// ��������: MsgDrvTask
// ��������: ͨ���̡߳����շ�������������������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
TTaskRet MsgDrvTask(void *pvArg)
{
	TRACE("MsgDrvTask::Run().\n");

	unsigned int dwTxLen = 0;
	unsigned int dwRxLen = 0;
	unsigned char bConStatus = 0;
	unsigned int dwClick = 0;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return 0;
	unsigned char *pTxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pTxBuf == NULL)return 0;
	memset(pRxBuf, 0, APP_CCT_BUF_SIZE);
	memset(pTxBuf, 0, APP_CCT_BUF_SIZE);

	APP_MSG_ID_ENUM tMsgId;
	unsigned int dwMsgRxLen = 0;
	unsigned char *pMsgRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return 0;
	memset(pMsgRxBuf, 0, APP_CCT_BUF_SIZE);

	//ҵ��������
	pAppAdapter = new CCctAppAdapter();
	if (pAppAdapter == NULL) return 0;
	pAppAdapter->Init();
	bConStatus = FSM_CON_STATUS_TRAN;

	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_10MSEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_DRVRCV;
	tOnTimer.dwMsgId = APP_MSG_ID_DRV_10MS;
	//10ms��ʱ��
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_DRVRCV;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);

	for (;;)
	{

		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_DRVRCV, &tMsgId, pMsgRxBuf, &dwMsgRxLen, 0xffffffff))
		{

			switch(tMsgId)
			{
			case APP_MSG_ID_DRV_10MS:
#if 1
				if (!pAppAdapter->IsAdapterConnected())
				{
					//���ߵȴ�30S������ �ӿڡ�������
					//Sleep(5000);
					if (dwClick + 5 < GetClick())
					{
						dwClick = GetClick();
						pAppAdapter->ConnectAdapter();
						continue;
					}

				}
				else
				{
					//���շ�������������
					if (true == pAppAdapter->RcvFrm(pRxBuf, &dwRxLen))
					{
						TRACEHEX("==>CSocketIf::SocketRecv", pRxBuf, dwRxLen);
						AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REQ, pRxBuf, dwRxLen);
					}
			
#else
			
					if (CheckSvrLinkFsm())
					{
						//���շ�������������
						if (true == pAppAdapter->RcvFrm(pRxBuf, &dwRxLen))
						{
							TRACEHEX("==>SocketRcv", pRxBuf, dwRxLen);
							AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REQ, pRxBuf, dwRxLen);
						}
					}
#endif	
				}

				break;

			case APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE:
				MsgDrvTaskKeepAlive();
				break;

			default:
				TRACE("MsgDrvTask MsgId err !.\n");
				break;
				
			}

		//Sleep(10);
		
		}
		
	}

	return 0;
}

//*****************************************************************************
// ��������: MsgDrvTaskKeepAlive
// ��������: �����ֹ�����ļ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void MsgDrvTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

#if 0
//*****************************************************************************
// ��������: CheckSvrLinkFsm
// ��������: ͨ����·״̬ά��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
bool CheckSvrLinkFsm()
{
	static unsigned char m_pIfReCon = 0;
	static unsigned char m_pAdapterReCon = 0;
	unsigned char bConStatus = FSM_CON_STATUS_DISCON;

	for (;;)
	{
		switch(bConStatus)
		{
		case FSM_CON_STATUS_DISCON:
			if (!pAppAdapter->m_pIf->IsConnected())
			{
				pAppAdapter->m_pIf->SocketReConnect();
				if (pAppAdapter->m_pIf->IsConnected())
				{
					bConStatus = FSM_CON_STATUS_IF_CON;
					m_pIfReCon = 0;
				}
				else
				{
					m_pIfReCon++;
					if (m_pIfReCon > 3)
					{
						m_pIfReCon = 0;
						bConStatus = FSM_CON_STATUS_DISCON;
						Sleep(300000);
					}
					
					Sleep(5000);
					continue;
				}
			}
			else
			{
				bConStatus = FSM_CON_STATUS_IF_CON;
			}
			break;
			
		case FSM_CON_STATUS_IF_CON:
			if (!pAppAdapter->IsAdapterConnected())
			{
				pAppAdapter->ConnectAdapter();
				if (pAppAdapter->IsAdapterConnected())
				{
					bConStatus = FSM_CON_STATUS_TRAN;
					m_pAdapterReCon = 0;
				}
				else
				{
					m_pAdapterReCon++;
					if (m_pAdapterReCon > 3)
					{
						m_pAdapterReCon = 0;
						bConStatus = FSM_CON_STATUS_TRAN;
						Sleep(300000);
					}
					
					Sleep(5000);
					continue;
				}
			}
			else
			{
				bConStatus = FSM_CON_STATUS_TRAN;
			}
			break;
			
		case FSM_CON_STATUS_TRAN:
			return true;
			break;
			
		default:
		
			break;
		
		}
	}


}
#endif

//*****************************************************************************
// ��������: MsgSnedTask
// ��������: ͨ���̡߳��������ݵ�������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
TTaskRet MsgSendTask(void *pvArg)
{
	TRACE("MsgSendTask::Run().\n");

	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwTxLen = 0;

	unsigned char *pTxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pTxBuf == NULL)return 0;

	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_DRV;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);


	for (;;)
	{
		if (NULL != pAppAdapter)
		{
			if (!pAppAdapter->IsAdapterConnected())
			{
				Sleep(1000);
				continue;
			}
		}
		else
		{
			Sleep(1000);
			continue;
		}
	
		//������վ������ ���ߺ������ڶ�������
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_DRV, &dwMsgId, pTxBuf, &dwTxLen, 0xffffffff))
		{
			switch (dwMsgId)
			{
			case APP_MSG_ID_SVR_REP:
				TRACEHEX("<==CSocketIf::SocketSend", pTxBuf, dwTxLen);
				pAppAdapter->Send(pTxBuf, dwTxLen);
				break;

			case APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE:
				MsgSendTaskKeepAlive();
				break;

			default:
				TRACE("MsgSendTask MsgId err !.\n");
				break;
			}
		}

		//Sleep(10);
	}

}


//*****************************************************************************
// ��������: MsgSendTaskKeepAlive
// ��������: �����ֹ�����ļ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void MsgSendTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

//*****************************************************************************
// ��������: CctDbMgrTask
// ��������: ���ݿ����
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
TTaskRet CctDbMgrTask(void *pvArg)
{
	if (g_pDbMgr == NULL)
	{
		TRACE("DbgInit Fail.\n");
		return 0;
	}

	//g_pDbMgr->Run(pvArg);

	return 0;
}

//*****************************************************************************
// ��������: CctReaderTask
// ��������: PLC��������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
TTaskRet CctReaderTask(void *pvArg)
{
	if (g_pCctReader[CCTPLC_READER] != NULL)
	{
		//������
		g_pCctReader[CCTPLC_READER]->Run(pvArg);
	}

	return 0;
}

//*****************************************************************************
// ��������: CctReciveTask
// ��������: PLC�����������ݽ���
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
TTaskRet CctReciveTask(void *pvArg)
{
	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_10MSEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_CCTRCV;
	tOnTimer.dwMsgId = APP_MSG_ID_DRV_10MS;
	//10ms��ʱ��
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_CCTRCV;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);


	if (g_pCctReader[CCTPLC_READER] != NULL)
	{
#if defined(PRODUCT_CFG_PRODUCT_TYPE_MRS)
		g_pCctReader[CCTPLC_READER]->DoCctRcvFrm();
#endif

#if defined(PRODUCT_CFG_PRODUCT_TYPE_CSG)
		g_pCctReader[CCTPLC_READER]->DoCsgCctRcvFrm();
#endif

	}

	return 0;
}


//*****************************************************************************
// ��������: OnTimerTask
// ��������: ��ʱ������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
TTaskRet OnTimerTask(void *pvArg)
{
	TRACE("OnTimerTask::Run().\n");
	memset(g_tOnTimer, 0, sizeof(g_tOnTimer));

	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwRxLen = 0;
	unsigned int dwClick = 0;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return 0;

	TTOnTimer *ptOnTimer = NULL;

	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_TIM;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);

	for (;;)
	{	
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_TIM, &dwMsgId, pRxBuf, &dwRxLen, 0))
		{
			ptOnTimer = (TTOnTimer *)pRxBuf;

			switch (dwMsgId)
			{
				//��Ӷ�ʱ��
			case APP_MSG_ID_REG_ONTIMER:
				AddOnTimer((TTOnTimer *)pRxBuf);
				break;

				//ɾ����ʱ��
			case APP_MSG_ID_DEL_ONTIMER:
				DeleteOnTimer(ptOnTimer);
				break;

			case APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE:
				OnTimerTaskKeepAlive();
				break;

			default:
				TRACE("OnTimerTask MsgId err.\n");
				break;
			}
		}

		CheckTimeOver();

		//�����ʱ�����ȿ��ܲ����� ���´�ӡ�����ڵ���10ms��ʱ������
		//TRACE("OnTimerTask %d\n", GetTick());

		//�����ӳ���Ҫ��������֤10MS��ʱ���ڵ�ǰ�߳����ȼ��µľ�׼��
		Sleep(8);
	}

	return 0;
}

//*****************************************************************************
// ��������: MonitorTask
// ��������: �̼߳��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
TTaskRet MonitorTask(void *pvArg)
{
	TRACE("MonitorTask::Run().\n");
	unsigned int dwClick = 0;
	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwSize = 0;

	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_MINITOR;
	tOnTimer.dwMsgId = APP_MSG_ID_SEC_ONTIMER;
	RegisterOnTimer(&tOnTimer);	
	
	for (;;)
	{	
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_MINITOR, (APP_MSG_ID_ENUM *)&dwMsgId, (unsigned char *)&dwClick, &dwSize, 0xffffffff))
		{
			continue;
			//TRACE("dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
			
			switch(dwMsgId)
			{
			case APP_MSG_ID_CCT_DBMGR_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_DBMGR_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;

			case APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;
					
			case APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;
				
			case APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;
				
			case APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;

			case APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;

			case APP_MSG_ID_CCT_MAIN_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_MAIN_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;

			case APP_MSG_ID_SEC_ONTIMER:
				TRACE("MonitorTask APP_MSG_ID_SEC_ONTIMER.\n");
				break;
				
			default:
				TRACE("MonitorTask MsgId err.\n");
				break;
			}
		}
	
		//TRACE("MonitorTask.\n");
		//Sleep(10);
	}

	return 0;
}


//*****************************************************************************
// ��������: SysInit
// ��������: ϵͳʹ�������Դ��ʼ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void SysInit()
{
	InitDebug();

	CCT_APP_INFO tAppVersion;
	memcpy(&tAppVersion, &g_AppVersion, sizeof(CCT_APP_INFO));

	TRACE("/**************************************************/\n");
	TRACE("/***        LQBS_CCT_APP@www.richnano.cn        ***/\n");
	TRACE("/***        LQBS INTELLIGENT LTE MODULE         ***/\n");
#if defined(PRODUCT_CFG_PRODUCT_TYPE_MRS)	
	TRACE("/***        PRODUCT_CFG_PRODUCT_TYPE_MRS        ***/\n");
#endif
#if defined(PRODUCT_CFG_PRODUCT_TYPE_CSG)	
	TRACE("/***        PRODUCT_CFG_PRODUCT_TYPE_CSG        ***/\n");
#endif	
	TRACE("/***        APPVERSION: %d-%d-%d %d:%d:%d        ***/\n", 
							tAppVersion.tTime.wYear, 
							tAppVersion.tTime.bMonth,
							tAppVersion.tTime.bDay, 
							tAppVersion.tTime.bHour, 
							tAppVersion.tTime.bMinute, 
							tAppVersion.tTime.bSecond);
	TRACE("/**************************************************/\n\n");

	InitVar();

	//��ʼ����Ϣ����
	InitQueue();

	InitDb();

	InitCctReader();
	
	//�̼߳��
	InitTaskMonitor();	

}

//*****************************************************************************
// ��������: InitVar
// ��������: ȫ�ֱ�����ʼ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void InitVar()
{
	;
}

//*****************************************************************************
// ��������: InitDb
// ��������: ���ݿ��ʼ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void InitDb()
{
	g_pDbMgr = new CDbMgr();

	if (g_pDbMgr == NULL)
	{
		TRACE("DbgInit Fail.\n");
		return;
	}

	GetDbPara(&tCctDbPara);
	//g_pDbMgr->Init(&tCctDbPara);
}



//*****************************************************************************
// ��������: InitCctReader
// ��������: ����������ʵ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
bool InitCctReader()
{
	//������	
	//PLC������ʵ��
	CHisiReader *pCctReader = new CHisiReader();
	if (pCctReader == NULL)
	{
		TRACE("InitCctReader: Fail to new CHisiReader!\r\n");
		return false;
	}

	//װ�ز���
	LoadCctPara(ROUTER_LINK_HISI, &tCctPara);
	pCctReader->Init(&tCctPara);

	g_pCctReader[CCTPLC_READER] = (CCctReader*)pCctReader;

	return true;
}

//*****************************************************************************
// ��������: DbgInit
// ��������: ��ʼ�������������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void InitDebug()
{
	;
}

//*****************************************************************************
// ��������: InitTaskMonitor
// ��������: ����������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void InitTaskMonitor()
{

	;
}

