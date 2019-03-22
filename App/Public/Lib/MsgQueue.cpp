//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : MsgQueue.cpp
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

#include "MsgQueue.h"

signed int g_QueueHandle[APP_QUEUE_ID_MAX] = { 0 };						//消息队列句柄

const char *g_pszQueNmae[APP_QUEUE_ID_MAX] = { "APP_QUEUE_ID_MAIN",		//主线程队列
												"APP_QUEUE_ID_DRV",		//通信线程对列
												"APP_QUEUE_ID_DRVRCV",	//通信线程对列-接收
												"APP_QUEUE_ID_DB",		//数据库管理线程对列
												"APP_QUEUE_ID_CCT",		//PLC抄表线程对列
												"APP_QUEUE_ID_CCTRCV",	//PLC抄表线程对列
												"APP_QUEUE_ID_TIM",		//定时器管理线程队列
												"APP_QUEUE_ID_LTE",
												"APP_QUEUE_ID_MINITOR"};


//*****************************************************************************
// 函数名称: InitQueue
// 功能描述: 消息队列创建
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
bool InitQueue()
{
	//初始化消息队列
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
// 函数名称: GetQueueName
// 功能描述: 获取消息队列名
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
const char *GetQueueName(signed int dwQueueId)
{

	return g_pszQueNmae[dwQueueId];
}

//*****************************************************************************
// 函数名称: QueueName2Id
// 功能描述: 获取消息队列ID
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
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
// 函数名称: GetQueueHandle
// 功能描述: 获取消息队收发句柄
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int GetQueueHandle(APP_QUEUE_ID_ENUM QueueId)
{

	return g_QueueHandle[QueueId];
}


//*****************************************************************************
// 函数名称: AppSendMessage2Queue
// 功能描述: 消息发送到消息队列
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
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

	//构造stMsg
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

	//消息净荷
	memcpy(pstFrame->pFrmData, pBuf, dwSize);
	pstFrame->dwFrmSize = dwSize;
	stMsg.dwMsgId = dwMsgId;
	stMsg.dwParam[0] = (unsigned long long)pstFrame;		//dwParam[0]存放消息body(长度域+数据域)地址
	return SendMessage2Queue(dwQueId, &stMsg, 0);
}


//*****************************************************************************
// 函数名称: AppRcvMessage
// 功能描述: 从消息队列接收消息
//
// 参数说明:
// dwQueId  -  in，pMsgId  -  out，pBuf  -  out，pSize  -  out
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
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

