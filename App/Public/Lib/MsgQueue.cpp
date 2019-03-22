//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : MsgQueue.cpp
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

#include "MsgQueue.h"

signed int g_QueueHandle[APP_QUEUE_ID_MAX] = { 0 };						//��Ϣ���о��

const char *g_pszQueNmae[APP_QUEUE_ID_MAX] = { "APP_QUEUE_ID_MAIN",		//���̶߳���
												"APP_QUEUE_ID_DRV",		//ͨ���̶߳���
												"APP_QUEUE_ID_DRVRCV",	//ͨ���̶߳���-����
												"APP_QUEUE_ID_DB",		//���ݿ�����̶߳���
												"APP_QUEUE_ID_CCT",		//PLC�����̶߳���
												"APP_QUEUE_ID_CCTRCV",	//PLC�����̶߳���
												"APP_QUEUE_ID_TIM",		//��ʱ�������̶߳���
												"APP_QUEUE_ID_LTE",
												"APP_QUEUE_ID_MINITOR"};


//*****************************************************************************
// ��������: InitQueue
// ��������: ��Ϣ���д���
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
bool InitQueue()
{
	//��ʼ����Ϣ����
	unsigned int dwQueueId = 0;
	signed short dwMsgArrayCnt = 512;

	for (signed short wQueue = APP_QUEUE_ID_MAIN; wQueue < APP_QUEUE_ID_MAX; wQueue++)
	{
		if (NULL != g_pszQueNmae[wQueue])
		{
			g_QueueHandle[wQueue] = NewQueue(wQueue, (char *)g_pszQueNmae[wQueue], dwMsgArrayCnt);

			if (g_QueueHandle[wQueue] == 0)
			{
				TRACE("InitQueue %s Fail.\n", g_pszQueNmae[wQueue]);
				return false;
			}
		}
	}

	return true;
}

//*****************************************************************************
// ��������: GetQueueName
// ��������: ��ȡ��Ϣ������
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
const char *GetQueueName(signed int dwQueueId)
{

	return g_pszQueNmae[dwQueueId];
}

//*****************************************************************************
// ��������: QueueName2Id
// ��������: ��ȡ��Ϣ����ID
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
bool QueueName2Id(const char *pszName, signed int *pdwQueueId)
{
	if (pszName == NULL)
		return false;

	for (signed int dwQueueId = 0; dwQueueId < APP_QUEUE_ID_MAX; dwQueueId++)
	{

		if (0 == strcmp(g_pszQueNmae[dwQueueId], pszName))
		{
			*pdwQueueId = dwQueueId;
			return true;
		}
	}

	return false;
}


//*****************************************************************************
// ��������: GetQueueHandle
// ��������: ��ȡ��Ϣ���շ����
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
signed int GetQueueHandle(APP_QUEUE_ID_ENUM QueueId)
{

	return g_QueueHandle[QueueId];
}


//*****************************************************************************
// ��������: AppSendMessage2Queue
// ��������: ��Ϣ���͵���Ϣ����
//
// ����˵��:
// dwQueId  -  in��dwMsgId  -  in��pBuf  -  in��dwSize  -  in
// 
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed int AppSendMessage2Queue(APP_QUEUE_ID_ENUM dwQueId, APP_MSG_ID_ENUM dwMsgId, unsigned char *pBuf, unsigned int dwSize)
{
	if ((pBuf == NULL) ||
		(0 == dwSize) ||
		(dwQueId > APP_QUEUE_ID_MAX) ||
		(dwMsgId > APP_MSG_ID_MAX))
	{
		return (-1);
	}

	//����stMsg
	CCT_QUEUE_MSG_S stMsg = { 0 };
	memset(&stMsg, 0, sizeof(CCT_QUEUE_MSG_S));
	CCT_MSG_ONE_FRM_S *pstFrame = (CCT_MSG_ONE_FRM_S *)new char[sizeof(CCT_MSG_ONE_FRM_S) + dwSize];
	if (NULL == pstFrame)return (-1);
	pstFrame->pFrmData = (unsigned char *)pstFrame + sizeof(CCT_MSG_ONE_FRM_S);
	if (pstFrame->pFrmData == NULL) 
	{
		delete(pstFrame);
		return (-1);
	}

	//��Ϣ����
	memcpy(pstFrame->pFrmData, pBuf, dwSize);
	pstFrame->dwFrmSize = dwSize;
	stMsg.dwMsgId = dwMsgId;
	stMsg.dwParam[0] = (unsigned long long)pstFrame;		//dwParam[0]�����Ϣbody(������+������)��ַ
	return SendMessage2Queue(dwQueId, &stMsg, 0);
}


//*****************************************************************************
// ��������: AppRcvMessage
// ��������: ����Ϣ���н�����Ϣ
//
// ����˵��:
// dwQueId  -  in��pMsgId  -  out��pBuf  -  out��pSize  -  out
// 
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed int AppRcvMessage(APP_QUEUE_ID_ENUM dwQueId, APP_MSG_ID_ENUM *pMsgId, unsigned char *pBuf, unsigned int *pSize, unsigned int dwTimeOut)
{
	if ((pBuf == NULL) ||
		(NULL == pSize) ||
		(dwQueId > APP_QUEUE_ID_MAX) ||
		(pMsgId == NULL))
	{
		return (-1);
	}

	CCT_QUEUE_MSG_S stMsg = { 0 };
	memset(&stMsg, 0, sizeof(CCT_QUEUE_MSG_S));
	if (0 == WaitMessage(dwQueId, &stMsg, dwTimeOut))
	{
		*pMsgId = (APP_MSG_ID_ENUM)stMsg.dwMsgId;
		CCT_MSG_ONE_FRM_S *p = (CCT_MSG_ONE_FRM_S *)(stMsg.dwParam[0]);
		if (p != NULL)
		{
			if (NULL != p->pFrmData)
			{
				*pSize = p->dwFrmSize;
				memcpy(pBuf, p->pFrmData, *pSize);
				delete p;
				p = NULL;
				return 0;
			}
		}
	}

	return (-1);
}

