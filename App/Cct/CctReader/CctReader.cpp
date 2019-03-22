//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : CctReader.cpp
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 抄表
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************
#include "CctReader.h"
#include "CctCfg.h"
#include "DbCfg.h"
#include "CctAppAdapter.h"
#include "DbMgr.h"

CCctReader::CCctReader()
{

}


CCctReader:: ~CCctReader()

{

}

//*****************************************************************************
// 函数名称: Init
// 功能描述: 
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
bool CCctReader::Init(TCctPara *pCctPara)
{
	m_fStopRdMtr = false;
	m_fStopRouter = false;
	m_fInReadPeriod = true;	//是否在抄表时段内
	m_fSchMtr = false;		//不在搜索状态
	m_fInInterVRead = false;
	m_fDirectOpera = false;	//外界的直接操作
	m_wRdTurn = 0;			//轮抄次数
	m_dwStudyClick = 0;		//学习时间
	m_bLinkType = pCctPara->bLink;
	m_fPlcMoudleChange = false;
	m_dwDirReadClick = 0;

	m_pInBuf = new unsigned char[1024];
	m_pOutBuf = new unsigned char[1024];
	m_pTx = new unsigned char[1024];
	m_pRx = new unsigned char[1024];
	m_RxLen = 1024;
	m_TxLen = 1024;
	m_dwInBufLen = 1024;
	m_dwOutBufLen = 1024;

	m_bRxSN = 0;			//发送报文序列号
	m_bTxSN = 0;			//接受报文序列号

	m_wTaskNo = 1;			//南网任务号			
	m_wTaskTimeOut = 60;	//南网超时时长
	memset(m_bMainAddr,0xee, sizeof(m_bMainAddr));
	m_fIsMainNodeAddrOk = false;
	m_dwUartPulseClick = GetClick();

	//hfg-0120加记录到数据库
	//if (NULL != g_pDbMgr)
	//	g_pDbMgr->CreateTables();

	m_pComm = new CComm();

	if ((m_pInBuf == NULL) ||
		(m_pOutBuf == NULL) ||
		(m_pTx == NULL) ||
		(m_pRx == NULL) ||
		(m_pComm == NULL))
		return false;

	SetCctPara(pCctPara);
	m_pComm->Init(pCctPara->RdrPara.tCommPara);
	return true;
}

//*****************************************************************************
// 函数名称: Run
// 功能描述: 
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
void CCctReader::Run(void *pvArg)
{

	TRACE("CctReaderTask::Run().\n");

	signed int dwRet = 0;

	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwRxLen = 0;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return;
	memset(pRxBuf, 0, APP_CCT_BUF_SIZE);


	TTOnTimer tOnTimer;

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_CCT;
	tOnTimer.dwMsgId = APP_MSG_ID_SEC_ONTIMER;
	//注册秒任务定时器
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_CCT;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE;
	//线程的心跳检测
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_UART_PULSE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_CCT;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE;
	//串口死机检测
	//RegisterOnTimer(&tOnTimer);

	while (!m_fIsMainNodeAddrOk)
	{
#if defined(PRODUCT_CFG_PRODUCT_TYPE_MRS)	
		ReadMainAddr();
#endif		
#if defined(PRODUCT_CFG_PRODUCT_TYPE_CSG)	
		ReadCsgMainAddr();
#endif
		Sleep(5000);
	}

	for (;;)
	{
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_CCT, &dwMsgId, pRxBuf, &dwRxLen, 0xffffffff))
		{
			CctReaderMsgProc(dwMsgId, pRxBuf, dwRxLen);
		}

		//Sleep(10);
	}

}

//*****************************************************************************
// 函数名称: CctReaderMsgProc
// 功能描述: 
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
signed int CCctReader::CctReaderMsgProc(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize)
{

	if ((NULL == pFrmData) || (0 == dwFrmSize))
	{
		return (-1);
	}

	switch (dwMsgId)
	{
		//主站过来的抄表请求
	case APP_MSG_ID_SVR_REQ:
	case APP_MSG_ID_DB_REQ:
		//TRACE("CctReaderMsgProc send APP_MSG_ID_SVR_REP to APP_QUEUE_ID_DRV.\n");
		//被动型路由 AFN=1301
		DoCctReader(dwMsgId, pFrmData, dwFrmSize);
		break;

	case APP_MSG_ID_CCT_TOP_REQ:
		GetCctReaderCcoTop();
		break;

	case APP_MSG_ID_SEC_ONTIMER:
		DoCctUpdate();
		break;
		
	case APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE:
		CctReaderTaskKeepAlive();
		break;

	case APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE:
		//CctReaderTaskUartPulse();
		break;

	case APP_MSG_ID_CCT_CON:
		Send(pFrmData,dwFrmSize);
		break;

	default:

		break;
	}

	return 0;
}

//*****************************************************************************
// 函数名称: CctReaderTaskUartPulse
// 功能描述: 
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
void CCctReader::CctReaderTaskUartPulse()
{
	//unsigned char bPulseFrm[] = {0x68,0x0f,0x00,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x08,0x00,0x4e,0x16};
	unsigned char bPulseFrm[] = {0x68,0x0f,0x00,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x47,0x16};		//确认/否认/心跳-扩展F3
	
	if (CCT_ERR_SUCCESS == Send(bPulseFrm, sizeof(bPulseFrm)))
	{
		unsigned int dwClick = GetClick();
		AppSendMessage2Queue(APP_QUEUE_ID_CCTRCV, APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE, (unsigned char *)&dwClick, sizeof(unsigned int));
	}
}


//*****************************************************************************
// 函数名称: CctReaderTaskKeepAlive
// 功能描述: 软件防止死机的检测
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
void CCctReader::CctReaderTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

//*****************************************************************************
// 函数名称: CctRecvTaskKeepAlive
// 功能描述: 软件防止死机的检测
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
void CCctReader::CctRecvTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

//*****************************************************************************
// 函数名称: GetCctReaderCcoTop
// 功能描述: 查询CCo拓扑结构数据
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
void CCctReader::GetCctReaderCcoTop()
{
	unsigned int dwLen = 0;
	TTopPara tTopPara;
	tTopPara.wNodeSeq = 1;
	tTopPara.bNodeNum = 64;

	Make3762Frm(m_pTx, &dwLen, CCT_AFN_3762_QRYRT, FN228, (unsigned char *)&tTopPara, sizeof(TTopPara));
	Send(m_pTx, dwLen);
}


//*****************************************************************************
// 函数名称: DoCctRcvFrm
// 功能描述: 
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
void CCctReader::DoCctRcvFrm()
{
	TRACE("CctReciveTask::Run().\n");

	unsigned int dwRxLen = 0;
	APP_MSG_ID_ENUM dwMsgId;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return;
	memset(pRxBuf, 0, APP_CCT_BUF_SIZE);

	unsigned char bAfn = 0;
	for (;;)
	{
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_CCTRCV, &dwMsgId, pRxBuf, &dwRxLen, 0xffffffff))
		{
			switch(dwMsgId)
			{
			case APP_MSG_ID_DRV_10MS:
				if (true == CCctReader::RcvFrm(m_pRx, &m_RxLen))
				{
					bAfn = GetAfn(m_pRx, m_RxLen);
				
					switch (bAfn)
					{
					//路由参数
					case CCT_AFN_3762_QRYDATA:
						DoDispatchParaFrm(m_pRx, m_RxLen);
						break;
					
					case CCT_AFN_3762_RTFWD:
						DoDispatchFrm(m_pRx, m_RxLen);
						break;
				
					case CCT_AFN_3762_REP:
						DoAutoRepFrm(m_pRx, m_RxLen);
						break;
				
					case CCT_AFN_3762_QRYRT:
						DoRouterQueryFrm(m_pRx, m_RxLen);
						break;
						
					case CCT_AFN_3762_CON:
						DoRouterConFrm(m_pRx, m_RxLen);
						break;
				
					default:
						TRACE("DoCctRcvFrm bAfn %x not DoDisPatch.\n", bAfn);
						break;
				
					}
				}
				break;
				
			case APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE:
				CctRecvTaskKeepAlive();
				break;

			case APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE:
				CheckUartPulse(pRxBuf);
				break;
				
			default:
			
				break;
			}
		

		}

		//Sleep(10);

	}

}

void CCctReader::CheckUartPulse(unsigned char *pBuf)
{
	unsigned dwClick = *(unsigned int *)pBuf;

	//超过两个心跳周期未收到串口发送的数据
	if (dwClick > m_dwUartPulseClick + TASK_UART_PULSE_CHECK_TIME *2)
	{
		//SystemReboot();
	}

}

//*****************************************************************************
// 函数名称: DoRouterConFrm
// 功能描述: 
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
void CCctReader::DoRouterConFrm(unsigned char *pBuf, unsigned short wLen)
{
	unsigned short wFn = 0;
	bool fAddrRegion = false;
	unsigned char bDataPos = 0;
	unsigned short wOutBufLen = 0;


	if (IsAddrRegion(pBuf, wLen))
	{
		wFn = DtToFn(pBuf + 23);
		fAddrRegion = true;
	}
	else
	{
		wFn = DtToFn(pBuf + 11);
	}

	//串口心跳
	if (wFn == FN3)
	{
		m_dwUartPulseClick = GetClick();
	}

}

//*****************************************************************************
// 函数名称: DoDispatchParaFrm
// 功能描述: 
//
// 参数说明: 路由参数 AFN=03 FN=04
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
signed int CCctReader::DoDispatchParaFrm(unsigned char *pInBuf, unsigned short wInLen)
{
	unsigned short wFn = 0;

	TRACE("DoDispatchParaFrm.\n");

	if ((pInBuf == NULL) || (wInLen == 0))
	{
		TRACE("DoDispatchParaFrm err !.\n");
		return (-1);
	}

	if (IsAddrRegion(pInBuf, wInLen))
	{
		wFn = DtToFn(pInBuf + 23);
	}
	else
	{
		wFn = DtToFn(pInBuf + 11);
	}

	if (wFn == FN4)
	{
		if (!IsAddrRegion(pInBuf, wInLen))
		{
			memcpy(m_bMainAddr, pInBuf + 13, TERMINAL_ADDR_SIZE);
		}
		else
		{
			memcpy(m_bMainAddr, pInBuf + 25, TERMINAL_ADDR_SIZE);
		}
		
		if (//IsBufferAllVal(m_bMainAddr, TERMINAL_ADDR_SIZE, 0) ||
			IsBufferAllVal(m_bMainAddr, TERMINAL_ADDR_SIZE, 0xee) ||
			IsBufferAllVal(m_bMainAddr, TERMINAL_ADDR_SIZE, 0xff) ||
			IsBufferAllVal(m_bMainAddr, TERMINAL_ADDR_SIZE, 0x99))
		{
			TRACE("DoDispatchParaFrm err !.\n");
			return (-1);		
		}
		
		m_fIsMainNodeAddrOk = true;
		TRACEHEX("m_bMainAddr", m_bMainAddr, TERMINAL_ADDR_SIZE);
		return AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_CCT_MAINNODE, m_bMainAddr, TERMINAL_ADDR_SIZE);
		
	}

	return CCT_ERR_SUCCESS;

}

//*****************************************************************************
// 函数名称: DoAutoRepFrm
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	  者: sUesTcxF
//*****************************************************************************
void CCctReader::DoAutoRepFrm(unsigned char *pBuf, unsigned short wLen)
{
	unsigned short wFn = 0;
	bool fAddrRegion = false;
	unsigned char bDataPos = 0;	
	unsigned char *p = m_pRx;
	TTime tTime;
	unsigned short wOutBufLen = 0;


	if (IsAddrRegion(pBuf, wLen))
	{
		wFn = DtToFn(pBuf + 23);
		fAddrRegion = true;
	}
	else
	{
		wFn = DtToFn(pBuf + 11);
	}

	//上报停电事件
	if (wFn == FN5)
	{
		if (fAddrRegion)
		{
			bDataPos = 29;
		}
		else
		{
			bDataPos = 17;
		}

		GetTime(&tTime);
		unsigned short wDataSize = (p[2] << 8) + p[1] - 19;


		for (unsigned short i = 0; i < wDataSize / 6; i++)
		{
			*m_pOutBuf = 1;		//停电ERC
			wOutBufLen++;
			memcpy(m_pOutBuf + 1, (unsigned char *)p + bDataPos, 6);	//ADDR
			wOutBufLen += 6;
			p += 6;

			memcpy(m_pOutBuf + wOutBufLen, &tTime, sizeof(TTime));
			wOutBufLen += sizeof(TTime);

			AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_EVENT_DATA, m_pOutBuf, wOutBufLen);
			TRACEHEX("DoAutoRepFrm PowerOff Event", m_pOutBuf, wOutBufLen);
			wOutBufLen = 0;
		}

		CctConfirm();

	}
}

//*****************************************************************************
// 函数名称: CctConfirm
// 功能描述: 
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
void CCctReader::CctConfirm()
{
	unsigned char bBuf[64];
	unsigned int dwLen = 0;
	memset(bBuf, 0, sizeof(bBuf));
	if (true == Make3762Frm(bBuf, &dwLen, 0, 1, NULL, 0))
	{
		AppSendMessage2Queue(APP_QUEUE_ID_CCT, APP_MSG_ID_CCT_CON, bBuf, dwLen);
	}
}


//*****************************************************************************
// 函数名称: DoAutoRepFrm
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	  者: sUesTcxF
//*****************************************************************************
void CCctReader::DoRouterQueryFrm(unsigned char *pBuf, unsigned short wLen)
{
	TRACE("DoRouterQueryFrm.\n");
	unsigned short wFn = 0;
	bool fAddrRegion = false;
	unsigned char bDataPos = 0;
	unsigned char bAddrLen = 0;

	if (IsAddrRegion(pBuf, wLen))
	{
		wFn = DtToFn(pBuf + 23);
		fAddrRegion = true;
	}
	else
	{
		wFn = DtToFn(pBuf + 11);
	}


	if (wFn == FN228)
	{
		if (fAddrRegion)
		{
			bDataPos = 25;
			bAddrLen = 12;
		}
		else
		{
			bDataPos = 13;
			bAddrLen = 0;
		}

		m_dwOutBufLen = (m_pRx[2] << 8) + m_pRx[1] - 15 - bAddrLen;
		memcpy(m_pOutBuf, (unsigned char *)m_pRx + bDataPos, m_dwOutBufLen);

		TRACEHEX("DoRouterQueryTopInfo", m_pOutBuf, m_dwOutBufLen);
		AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_TOP_DATA, m_pOutBuf, m_dwOutBufLen);

	}
}

//*****************************************************************************
// 函数名称: IsAddrRegion
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
bool CCctReader::IsAddrRegion(unsigned char *pBuf, unsigned short wLen)
{
	if (0 != (*(pBuf + 4) & 0x04))
		return true;
	else
		return false;
}

//*****************************************************************************
// 函数名称: GetAfn
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
unsigned char CCctReader::GetAfn(unsigned char *pBuf, unsigned short wLen)
{
	if (IsAddrRegion(pBuf, wLen))
	{
		//带地址域
		return *(pBuf + 22);

	}
	else
	{
		//不带地址域
		return *(pBuf + 10);
	}

	return 0;
}

#define FRM_645_DATA_POS			29
//*****************************************************************************
// 函数名称: DoDispatchFrm
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
void CCctReader::DoDispatchFrm(unsigned char *pBuf, unsigned short wLen)
{
	if (CCT_ERR_SUCCESS == Get645FrmSizeFrom3762Frm(m_pRx, &m_dwOutBufLen))
	{
		if (0 != m_dwOutBufLen)
		{
			memcpy(m_pOutBuf, (unsigned char *)m_pRx + FRM_645_DATA_POS, m_dwOutBufLen);

			//TRACEHEX("MtrDataFrm", m_pOutBuf, m_dwOutBufLen);
			AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_645_DATA, m_pOutBuf, m_dwOutBufLen);
			//AppSendMessage2Queue(APP_QUEUE_ID_DB, APP_MSG_ID_DB_REP, m_pOutBuf, m_dwOutBufLen);
		}
	}
}

//*****************************************************************************
// 函数名称: GetMainAddr
// 功能描述: 
//
// 参数说明: 获取主节点地址
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
unsigned char* CCctReader::GetMainAddr()
{
	return m_bMainAddr;
}

//*****************************************************************************
// 函数名称: ReadMainAddr
// 功能描述: 
//
// 参数说明: 获取主节点地址
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
signed int CCctReader::ReadMainAddr()
{
	if (true == Make3762Frm(m_pTx, &m_TxLen, 0x03, FN4, NULL, 0))
	{
		if (CCT_ERR_SUCCESS == CCctReader::Send(m_pTx, m_TxLen))
		{
			return CCT_ERR_SUCCESS;
		}
	}

	return (-1);
}

//*****************************************************************************
// 函数名称: Get645FrmSizeFrom3762Frm
// 功能描述: 
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
signed int CCctReader::Get645FrmSizeFrom3762Frm(unsigned char *p3762Frm, unsigned int *p645FrmSize)
{
	if ((NULL == p3762Frm) || (p645FrmSize == NULL))
		return (-1);

	//(68+ADDR+68+C+L  +CS+16 = 12)
	//*p645FrmSize = GetDataLen(p3762Frm + FRM_645_DATA_POS, CCT_MTRPROTO_07) + 12;
	*p645FrmSize = (unsigned char)*(p3762Frm + 28);
	return CCT_ERR_SUCCESS;
}

//*****************************************************************************
// 函数名称: DoCctUpdate
// 功能描述: 状态更新
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
void CCctReader::DoCctUpdate()
{
	signed int iRet;

	LockReader();
	//只对被动抄表器有效,对于自动路由算法及485这样的主动抄表器无效
	//对于被动抄表模块来说,基本都是模块请求一帧,主终端回答一帧,
	//所以把整个过程锁定也不会影响系统对实时抄表或者遥控这样的命令的相应时间.
	//但是对于主动抄表模块来说,一般都是一个功能函数把所有电表都过一遍,所以
	//执行时间都比较长,它们只能把抄表操作串口部分进行局部锁定,以便能实时插入
	//对电表的操作

	iRet = UpdateCctReader();	//进行路由器更新

	switch (m_bState)
	{
	case ROUTER_STA_LEARN_ALL:
		iRet = DoLearnAll();
		break;

	case ROUTER_STA_LEARN_LOCAL:
		iRet = DoLearnLocal();
		break;

	case ROUTER_STA_READ:
		//主动型路由 AFN=1401
		iRet = DoAutoCctReader();
		break;

	case ROUTER_STA_OPTIMIZE:
		iRet = DoOptimize();
		break;

	case ROUTER_STA_IDLE:
		iRet = DoIdle();
		break;

	case ROUTER_STA_EXIT:
		UnLockReader();
		return;

	default:
		m_bState = ROUTER_STA_READ;
		break;
	}

	UnLockReader();

}

//被动(通道型)路由抄表 XC、RSC、HISI AFN=13 FN=01
//通道型路由不用处理采集器节点的一对多问题，
//直接电表地址加采集器地址组帧下发，不用做采集器的一对多的轮询
//*****************************************************************************
// 函数名称: DoCctReader
// 功能描述: 
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
void CCctReader::DoCctReader(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize)
{

	static unsigned short wPn = 0;
	signed int dwRet = 0;

	//TCctRwItem tRwIte;
	CCT_QUEUE_MSG_S pstMsg = { 0 };

	if (m_fStopRdMtr || !m_fInReadPeriod || m_fSchMtr || m_fInInterVRead)
	{
		return;
	}

	if (m_fStopRdMtr || m_fSchMtr || !IsInReadPeriod())
	{
		return;
	}

	//处于 停止抄表状态 || 不在抄表时段内 检测到模块被更换了
	if (m_fStopRdMtr || !IsInReadPeriod() || m_fSchMtr || m_fPlcMoudleChange)
	{
		return;
	}

	//StopRouter();

	//bBuf-传入645,dwBufLen
	m_dwOutBufLen = 0;
	dwRet = CctReadItemData(pFrmData, dwFrmSize, m_pOutBuf, &m_dwOutBufLen);

	//测试代码
	//unsigned char bdata[] = {0x68, 0x01, 0x00, 0x00, 0x09, 0x17, 0x20, 0x68, 0x91, 0x08, 0x33, 0x33, 0x34, 0x33, 0x57, 0x5a, 0x33, 0x33, 0x8e, 0x16};
	//AppSendMessage2Queue(APP_QUEUE_ID_DB, APP_MSG_ID_SVR_REP, bdata, sizeof(bdata));

	//ResumeRouter();

}

//*****************************************************************************
// 函数名称: StopRouter
// 功能描述: 
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
bool CCctReader::StopRouter()
{
	m_RxLen = 0;

	if (m_fStopRouter)
	{
		return true;
	}

	MakeCtrlFrm(m_pTx, &m_TxLen, CCT_AFN_3762_CTRLRT, FN1);
	TRACEHEX("Restart Router", m_pTx, m_TxLen);
	if (CCT_ERR_SUCCESS != CCctReader::Send(m_pTx, m_TxLen))
	{
		TRACE("CCctReader::StopRouter Fail.\n");
	}

	MakeCtrlFrm(m_pTx, &m_TxLen, CCT_AFN_3762_CTRLRT, FN2);
	TRACEHEX("Stop Router", m_pTx, m_TxLen);
	if (CCT_ERR_SUCCESS != CCctReader::Send(m_pTx, m_TxLen))
	{
		TRACE("CCctReader::StopRouter Fail.\n");
	}

	m_fStopRouter = true;

	return true;
}

//*****************************************************************************
// 函数名称: ResumeRouter
// 功能描述: 恢复路由抄表
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
bool CCctReader::ResumeRouter()
{
	m_RxLen = 0;

	if (!m_fStopRouter)
	{
		return true;
	}

	MakeCtrlFrm(m_pTx, &m_TxLen, CCT_AFN_3762_CTRLRT, FN3);
	TRACEHEX("Resume Router", m_pTx, m_TxLen);
	if (CCT_ERR_SUCCESS == CCctReader::Send(m_pTx, m_TxLen))
	{
		TRACE("CCctReader::CCctReader Fail.\n");
	}

	m_fStopRouter = false;

	return true;
}


//*****************************************************************************
// 函数名称: SetCctPara
// 功能描述: 
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
void CCctReader::SetCctPara(TCctPara *pCctPara)
{
	m_pCctPara = pCctPara;
}

//*****************************************************************************
// 函数名称: CtrlRoute
// 功能描述: 路由控制
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
bool CCctReader::CtrlRoute(unsigned short wFn)
{
	MakeCtrlFrm(m_pTx, &m_TxLen, CCT_AFN_3762_CTRLRT, wFn);
	Send(m_pTx, m_TxLen);
	return true;
}

//*****************************************************************************
// 函数名称: MakeCtrlFrm
// 功能描述: 
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
bool CCctReader::MakeCtrlFrm(unsigned char *pBuf, unsigned int *pdwLen, unsigned char bAFN, unsigned short wFn)
{
	unsigned char bLenBytes = m_pCctPara->wFrmLenBytes - 1;	//帧长度字节数	
	unsigned char bAddrLen = 12;							//主节点地址长度
	unsigned char bPRM = FRM_C_PRM_1;						//启动方
	unsigned char bMode = m_pCctPara->bLink;
	unsigned short wLen = 15;

	pBuf[0] = 0x68;
	pBuf[1] = (unsigned char)wLen;							//长度L
	pBuf[2] = (unsigned char)(wLen >> 8);					//长度L
	pBuf[2 + bLenBytes] = (unsigned char)(bPRM*(0x01 << 6) + bMode);
	pBuf[3 + bLenBytes] = 0;		//D2通信模块标识：0表示对集中器的通信模块操作;1表示对载波表的通信模块操作。
	pBuf[4 + bLenBytes] = 0;								//信道
	pBuf[5 + bLenBytes] = 0;
	pBuf[6 + bLenBytes] = 0;
	pBuf[7 + bLenBytes] = 0;
	pBuf[8 + bLenBytes] = 0;
	pBuf[9 + bLenBytes] = bAFN;
	FnToDt(wFn, &pBuf[10 + bLenBytes]);
	pBuf[12 + bLenBytes] = CheckSum(pBuf + 3, 10);
	pBuf[13 + bLenBytes] = 0x16;

	*pdwLen = wLen;

	return true;
}

//*****************************************************************************
// 函数名称: MakeCtrlFrm
// 功能描述: 
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
bool CCctReader::Make3762Frm(unsigned char *pBuf, unsigned int *pdwLen, unsigned char bAFN, unsigned short wFn, unsigned char *pInBuf, unsigned int dwLen)
{
	unsigned char bLenBytes = m_pCctPara->wFrmLenBytes - 1;	//帧长度字节数	
	unsigned char bAddrLen = 12;							//主节点地址长度
	unsigned char bPRM = FRM_C_PRM_1;						//启动方
	unsigned char bMode = m_pCctPara->bLink;
	unsigned short wLen = 15 + dwLen;

	pBuf[0] = 0x68;
	pBuf[1] = (unsigned char)wLen;							//长度L
	pBuf[2] = (unsigned char)(wLen >> 8);					//长度L
	pBuf[2 + bLenBytes] = (unsigned char)(bPRM*(0x01 << 6) + bMode);
	pBuf[3 + bLenBytes] = 0;		//D2通信模块标识：0表示对集中器的通信模块操作;1表示对载波表的通信模块操作。
	pBuf[4 + bLenBytes] = 0;								//信道
	pBuf[5 + bLenBytes] = 0;
	pBuf[6 + bLenBytes] = 0;
	pBuf[7 + bLenBytes] = 0;
	pBuf[8 + bLenBytes] = 0;
	pBuf[9 + bLenBytes] = bAFN;
	FnToDt(wFn, &pBuf[10 + bLenBytes]);
	if ((NULL != pInBuf) && (0 != dwLen))
	{
		memcpy(pBuf + 12 + bLenBytes, pInBuf, dwLen);
	}

	pBuf[12 + bLenBytes + dwLen] = CheckSum(pBuf + 3, 10 + dwLen);
	pBuf[13 + bLenBytes + dwLen] = 0x16;

	*pdwLen = wLen;

	return true;
}


//*****************************************************************************
// 函数名称: Make3762ReadFrm
// 功能描述: 
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
signed int CCctReader::Make3762ReadFrm(unsigned char* pbTxBuf, unsigned int *pdwLen,
	unsigned char bAfn, unsigned short wFn,
	unsigned char* pbData, unsigned short wDataLen)
{
	unsigned short wLen = 0;
	unsigned char bAddrLen = 12;							//主节点地址长度
	unsigned char bLenBytes = m_pCctPara->wFrmLenBytes - 1;	//帧长度字节数	
	unsigned short wAppDataFieldLen = 0;
	unsigned char bPRM = FRM_C_PRM_1;						//启动方
	unsigned char bCS = 0;

	if (pbData == NULL && wDataLen != 0)
	{
		TRACE("CCctReader::MakeFrm: fail.\n");
		return 0;
	}

	//1.头部
	pbTxBuf[0] = 0x68;

	//2.信息域R
	pbTxBuf[3 + bLenBytes] = (bAddrLen > 0) ? (1 << 2) : 0;		//D2通信模块标识：0表示对集中器的通信模块操作;1表示对载波表的通信模块操作。
	//pbTxBuf[4 + bLenBytes] = bCn;								//信道
	pbTxBuf[4 + bLenBytes] = 0;								//信道
	pbTxBuf[5 + bLenBytes] = 0;
	pbTxBuf[6 + bLenBytes] = 0;
	pbTxBuf[7 + bLenBytes] = 0;
	pbTxBuf[8 + bLenBytes] = 0;

	//3.协议数据
	//02 00 00 10   68 07 07 00 00 34 12 68 11 04 34 34 39 38 12 16
	unsigned char bData[100];
	memset(bData, 0, sizeof(bData));
	bData[0] = 2;											//电表协议类型97/07
	bData[1] = 0;											//通信延时相关性标志
	bData[2] = 0;											//从节点附属节点数量
	bData[3] = (unsigned char)wDataLen;						//协议数据长(645帧长)
	memcpy(bData + 4, pbData, wDataLen);

	//4.节点地址
	//目的节点地址，有两种方式 1.目的节点填写采集器地址 2.目的节点填写电表地址, 采用第2方式
	bool fIsAcq = false;
	unsigned char bMtrAddr[6] = { 0 };
	unsigned char bNodeAddr[6] = { 0 };
	memcpy(bMtrAddr, GetMtrAddr(pbData), 6);

	if (fIsAcq)
	{
		GetAcqAddr(bMtrAddr, bNodeAddr);
		SetAddrField(pbTxBuf, bNodeAddr);
	}
	else
	{
		SetAddrField(pbTxBuf, bMtrAddr);
	}

	//组应用数
	wAppDataFieldLen = MakeAppDataField(bAfn, wFn, bData, bData[3] + 4, &pbTxBuf[9 + bAddrLen + bLenBytes]);

	wLen = 6 + bAddrLen + wAppDataFieldLen + 5 + bLenBytes;
	pbTxBuf[1] = (unsigned char)wLen;						//长度L
	if (bLenBytes == 1)
	{
		pbTxBuf[2] = (unsigned char)(wLen >> 8);			//长度L
	}

	//模块类型
	unsigned char bMode = m_pCctPara->bLink;
	//控制域
	pbTxBuf[2 + bLenBytes] = (unsigned char)(bPRM*(0x01 << 6) + bMode); //控制域C

	//376.2帧序号
	unsigned char bSendSN = 0;
	if (IsNeedSeq(bAfn))
	{
		bSendSN = m_bRxSN;
	}
	else
	{
		bSendSN = m_bTxSN;
		m_bTxSN++;
	}
	pbTxBuf[8 + bLenBytes] = bSendSN;

	//计算校验
	bCS = 0;
	for (unsigned short i = 2 + bLenBytes; i < 2 + 1 + 6 + bAddrLen + wAppDataFieldLen + bLenBytes; i++)
	{
		bCS += pbTxBuf[i];
	}

	pbTxBuf[wAppDataFieldLen + bAddrLen + 9 + bLenBytes] = bCS;
	pbTxBuf[wAppDataFieldLen + bAddrLen + 10 + bLenBytes] = 0x16;

	*pdwLen = 6 + bAddrLen + wAppDataFieldLen + 5 + bLenBytes;

	return CCT_ERR_SUCCESS;

}

//*****************************************************************************
// 函数名称: IsNeedSeq
// 功能描述: 帧序号
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
bool CCctReader::IsNeedSeq(unsigned char bAfn)
{
	bool bIsRouterSN = false;

	if (bAfn == CCT_AFN_3762_CON || bAfn == CCT_AFN_3762_REP || bAfn == CCT_AFN_3762_RTRD)
	{
		bIsRouterSN = true;
	}

	return bIsRouterSN;
}


#define FRM_ADDR			10
//*****************************************************************************
// 函数名称: SetAddrField
// 功能描述: 
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
void CCctReader::SetAddrField(unsigned char *pbTxBuf, unsigned char *pbAddr)
{
	unsigned char bAddr[6] = { 0 };
	//GetTerminalAddr(bAddr);
	memset(bAddr, 0, sizeof(bAddr));		//主节点地址暂时填写0

	memcpy(&pbTxBuf[FRM_ADDR], bAddr, 6);
	memcpy(&pbTxBuf[FRM_ADDR + 6], pbAddr, 6);
}

//*****************************************************************************
// 函数名称: GetAcqAddr
// 功能描述: 用电表地址查采集器地址
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
void CCctReader::GetAcqAddr(unsigned char *pMtrAddr, unsigned char *pAcqAddr)
{
	pAcqAddr = pMtrAddr;
}

//*****************************************************************************
// 函数名称: FnToDt
// 功能描述: 
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
void CCctReader::FnToDt(unsigned short wFn, unsigned char* pbDt)
{
	pbDt[0] = 0x1 << ((wFn - 1) % 8);
	pbDt[1] = (unsigned char)((wFn - 1) / 8);
}

//*****************************************************************************
// 函数名称: DtToFn
// 功能描述: 
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
unsigned short CCctReader::DtToFn(unsigned char* pbDt)
{
	const static unsigned char bDtToFn[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	for (unsigned short i = 0; i < 8; i++)
	{
		if (pbDt[0] == bDtToFn[i])
		{
			return (unsigned short)pbDt[1] * 8 + i + 1;
		}
	}

	return 0;
}


//描述:组帧应用数据域
//*****************************************************************************
// 函数名称: MakeAppDataField
// 功能描述: 
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
unsigned short CCctReader::MakeAppDataField(unsigned char bAfn, unsigned short wFn,
	unsigned char* pbData, unsigned short wLen,
	unsigned char* pbDataField)
{
	pbDataField[0] = bAfn;
	FnToDt(wFn, &pbDataField[1]);
	memcpy(pbDataField + 3, pbData, wLen);

	return wLen + 3;
}

//*****************************************************************************
// 函数名称: RcvFrm
// 功能描述: 
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
#define FSM_STATUS_RX_BGEIN 		(0)
#define FSM_STATUS_RX_FRMHEAD		(1)
#define FSM_STATUS_RX_FRMLEN		(2)
#define FSM_STATUS_RX_FRMDATA		(3)
#define FRM_INF		        		(4) 	  //信息域R
#define RX_DATA_MAX_LEN				(2048)
bool CCctReader::RcvFrm(unsigned char *pRx, unsigned int *pRxLen)
{
	signed int dwRcvLen = 0;	//实际接收长度
	unsigned int dwRcvSize = 0;	//预计接收数据长度
	unsigned int dwLen = 0;		//总计接收数据长度
	unsigned int dwDataRegionLen = 0; //数据域长度
	unsigned int dwLastLen = 0;
	unsigned char  bRcvDataCount = 0;
	unsigned char  bRcvLenCount = 0;

	signed int ret = 0;
	unsigned char *pRxData = pRx;
	unsigned char ucCs = 0;
	unsigned char bStatus = FSM_STATUS_RX_BGEIN;

	for (;;)
	{
		switch (bStatus)
		{
		case FSM_STATUS_RX_BGEIN:
			dwRcvSize = 1;
			bStatus = FSM_STATUS_RX_FRMHEAD;
			break;

		case FSM_STATUS_RX_FRMHEAD:
			if (bRcvLenCount == 0)
			{
				if (pRx[0] == 0x68)
				{
					dwRcvSize = 2;
					bStatus = FSM_STATUS_RX_FRMLEN;
				}
			}
			else
			{
				//将剩下的收完
				dwRcvSize = dwLastLen;
				bStatus = FSM_STATUS_RX_FRMLEN;
			}

			break;

		case FSM_STATUS_RX_FRMLEN:
			if (bRcvDataCount == 0)
			{
				dwDataRegionLen = (pRx[2] << 8) + pRx[1] - 3; 					//减去长度域和头3个字节
				if (dwDataRegionLen > 0)
				{
					dwRcvSize = dwDataRegionLen;
					if (dwRcvSize > RX_DATA_MAX_LEN)
					{
						TRACE("dwDataRegionLen %d err len.\n", dwDataRegionLen);
						return false;
					}

					bStatus = FSM_STATUS_RX_FRMDATA;
				}
			}
			else
			{
				//将剩下的收完
				dwRcvSize = dwLastLen;
				bStatus = FSM_STATUS_RX_FRMDATA;
			}
			break;

		case FSM_STATUS_RX_FRMDATA:
			ucCs = CheckSum(pRx + 3, dwLen - 5);
			if ((pRx[dwLen - 1] == 0x16) && (pRx[dwLen - 2] == ucCs))
			{
				*pRxLen = dwLen;
				m_bRxSN = pRx[FRM_INF + 5];

				TRACEHEX("CCctReader::Recv<==", pRx, *pRxLen);
				return true;
			}
			else
			{
				//hfg-0120加记录到数据库
				//AddDebugPowerOffData2Db(0x31);
			}
			break;

		default:
			return false;
			break;

		}

		dwRcvLen = m_pComm->Read(pRxData, dwRcvSize);
		if (dwRcvLen != dwRcvSize)
		{
			if (bStatus == FSM_STATUS_RX_FRMLEN)
			{
				TRACE("CCctReader::RcvFrm FSM_STATUS_RX_FRMLEN ulRcvLen %d ulRcvSize %d usRcvCount %d.\n", dwRcvLen, dwRcvSize, bRcvLenCount);
				//TRACEHEX("CCctReader::RcvFrm", pRxData, dwRcvLen);
				Sleep(500);
				dwLastLen = dwRcvSize - dwRcvLen;
				bRcvLenCount++;
				if (bRcvLenCount > 5)
				{
					//hfg-0120加记录到数据库
					//AddDebugPowerOffData2Db(0x32);
					return false;
				}
				bStatus = FSM_STATUS_RX_FRMHEAD;
			}
			else if (bStatus == FSM_STATUS_RX_FRMDATA)
			{
				TRACE("CCctReader::RcvFrm FSM_STATUS_RX_FRMDATA ulRcvLen %d ulRcvSize %d usRcvCount %d.\n", dwRcvLen, dwRcvSize, bRcvDataCount);
				//TRACEHEX("CCctReader::RcvFrm", pRxData, dwRcvLen);
				Sleep(500);
				dwLastLen = dwRcvSize - dwRcvLen;
				bRcvDataCount++;
				if (bRcvDataCount > 5)
				{
					//hfg-0120加记录到数据库
					//AddDebugPowerOffData2Db(0x33);
					return false;
				}
				bStatus = FSM_STATUS_RX_FRMLEN;
			}
			else
			{
				return false;
			}

		}

		pRxData += dwRcvLen;
		dwLen += dwRcvLen;
	}

	return false;
}



//*****************************************************************************
// 函数名称: Send
// 功能描述: 
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
signed int CCctReader::Send(unsigned char *pTx, unsigned int dwTxLen)
{
	if (NULL == m_pComm)
	{
		TRACE("CCctReader::Send m_pComm NULL.\n");
		return (-1);
	}

	if (!m_pComm->IsOpen())
	{
		return (-1);
	}

	TRACEHEX("CCctReader::Send==>", pTx, dwTxLen);
	if (dwTxLen == m_pComm->Write(pTx, dwTxLen))
		return CCT_ERR_SUCCESS;
	else
		return (-1);

	return(-1);

}


//曲线数据抄读
//*****************************************************************************
// 函数名称: DoPointRead
// 功能描述: 
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
signed int CCctReader::DoPointRead()
{
	//分钟曲线数据抄读
	DoMinPointRead();

	//小时曲线数据抄读
	DoHourPointRead();

	return 0;
}

//*****************************************************************************
// 函数名称: DoMinPointRead
// 功能描述: 抄读分钟曲线数据
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
signed int CCctReader::DoMinPointRead()
{
	return 0;
}

//*****************************************************************************
// 函数名称: DoHourPointRead
// 功能描述: 抄读小时曲线数据
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
signed int CCctReader::DoHourPointRead()
{
	return 0;
}


///////////////////////////////////////////南网数据处理接口///////////////////////////////////////////
//*****************************************************************************
// 函数名称: DoCsgCctRcvFrm
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	  者: sUesTcxF
//*****************************************************************************
void CCctReader::DoCsgCctRcvFrm()
{
	TRACE("DoCsgCctRcvFrm::Run().\n");

	unsigned int dwRxLen = 0;
	APP_MSG_ID_ENUM dwMsgId;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return;
	memset(pRxBuf, 0, APP_CCT_BUF_SIZE);

	unsigned char bAfn = 0;
	for (;;)
	{
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_CCTRCV, &dwMsgId, pRxBuf, &dwRxLen, 0xffffffff))
		{
			switch(dwMsgId)
			{
			case APP_MSG_ID_DRV_10MS:
				if (true == CCctReader::RcvFrm(m_pRx, &m_RxLen))
				{
					bAfn = GetCsgAfn(m_pRx, m_RxLen);
				
					switch (bAfn)
					{
					case CCT_AFN_CSG_READ_PARA:
						DoDispatchCsgParaFrm(m_pRx, m_RxLen);
						break;
					
					case CCT_AFN_CSG_TASK_REP:
						DoCsgDispatchTaskRepFrm(m_pRx, m_RxLen, m_pTx, &m_TxLen);
						break;
						
					//case CCT_AFN_3762_CON:
					//	DoRouterConFrm(m_pRx, m_RxLen);
					//	break;
				
					default:
						TRACE("DoCctRcvFrm bAfn %x not DoDisPatch.\n", bAfn);
						break;
				
					}
				}
				break;
				
			case APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE:
				CheckUartPulse(pRxBuf);
				break;

			case APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE:
				CctRecvTaskKeepAlive();
				break;

			default:

				break;
			}
		}
	}

	//Sleep(10);

}


//*****************************************************************************
// 函数名称: IsCsgAddrRegion
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
bool CCctReader::IsCsgAddrRegion(unsigned char *pBuf, unsigned short wLen)
{
	if (0 != (*(pBuf + 3) & 0x40))
		return true;
	else
		return false;
}

//*****************************************************************************
// 函数名称: GetCsgAfn
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
unsigned char CCctReader::GetCsgAfn(unsigned char *pBuf, unsigned short wLen)
{
	if (IsCsgAddrRegion(pBuf, wLen))
	{
		//带地址域
		memcpy(m_bMainAddr, pBuf+10, sizeof(m_bMainAddr));
		return *(pBuf + 16);

	}
	else
	{
		//不带地址域
		return *(pBuf + 4);
	}
}

//*****************************************************************************
// 函数名称: GetCsgDI
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
unsigned int CCctReader::GetCsgDI(unsigned char *pBuf, unsigned short wLen)
{
	if (IsCsgAddrRegion(pBuf, wLen))
	{
		//带地址域
		return *(unsigned int *)(pBuf + 18);

	}
	else
	{
		//不带地址域
		return *(unsigned int *)(pBuf + 6);
	}
}

//*****************************************************************************
// 函数名称: ReadCsgMainAddr
// 功能描述: 
//
// 参数说明: 获取主节点地址
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
signed int CCctReader::ReadCsgMainAddr()
{
	if (CCT_ERR_SUCCESS == MakeCsgQueryMainNodeFrm(m_pTx, &m_TxLen, 0x03, 0xe8000303, NULL, 0))
	{
		if (CCT_ERR_SUCCESS == CCctReader::Send(m_pTx, m_TxLen))
		{
			return CCT_ERR_SUCCESS;
		}
	}

	return (-1);
}

//*****************************************************************************
// 函数名称: DoDispatchCsgParaFrm
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
signed int CCctReader::DoDispatchCsgParaFrm(unsigned char *pInBuf, unsigned short wInLen)
{
	if ((pInBuf == NULL) || (wInLen == 0))
	{
		return (-1);
	}

	int i = 0;
	unsigned int dwDI = GetCsgDI(pInBuf, wInLen);

	switch (dwDI)
	{
	case CCT_DI_CSG_PARA_DATA:
		GetCsgMainAddr(pInBuf, wInLen);
		break;

	default:
		TRACE("DoCsgDispatchParaFrm err.\n");
		break;

	}

	return CCT_ERR_SUCCESS;
}

//*****************************************************************************
// 函数名称: DoCsgDispatchParaFrm
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
signed int CCctReader::GetCsgMainAddr(unsigned char *pInBuf, unsigned short wInLen)
{
	TRACE("GetCsgMainAddr.\n");

	if ((pInBuf == NULL) || (wInLen == 0))
	{
		TRACE("GetCsgMainAddr err !.\n");
		return (-1);
	}

	if (!IsCsgAddrRegion(pInBuf, wInLen))
	{
		memcpy(m_bMainAddr, pInBuf + 10, TERMINAL_ADDR_SIZE);
	}
	else
	{
		memcpy(m_bMainAddr, pInBuf + 22, TERMINAL_ADDR_SIZE);
	}

	if (//IsBufferAllVal(m_bMainAddr, TERMINAL_ADDR_SIZE, 0) ||
		IsBufferAllVal(m_bMainAddr, TERMINAL_ADDR_SIZE, 0xee) ||
		IsBufferAllVal(m_bMainAddr, TERMINAL_ADDR_SIZE, 0xff) ||
		IsBufferAllVal(m_bMainAddr, TERMINAL_ADDR_SIZE, 0x99))
	{
		TRACE("GetCsgMainAddr err !.\n");
		return (-1);		
	}

	m_fIsMainNodeAddrOk = true;
	AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_CCT_MAINNODE, m_bMainAddr, TERMINAL_ADDR_SIZE);

	return CCT_ERR_SUCCESS;
}


//*****************************************************************************
// 函数名称: GetCsgDataFromTaskRepFrm
// 功能描述: 
//
// 参数说明:
//	This function has no arguments.
//
// 返 回 值:
//	  无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作	 者: sUesTcxF
//*****************************************************************************
signed int CCctReader::DoCsgDispatchTaskRepFrm(unsigned char *pInBuf, unsigned short wInLen,
																unsigned char *pOutBuf, unsigned int *pdwOutLen)
{
	if ((pInBuf == NULL) || (wInLen == 0) || 
		(pOutBuf == NULL) || (pdwOutLen == 0))
	{
		return (-1);
	}

	int i = 0;
	unsigned int dwDI = GetCsgDI(pInBuf, wInLen);

	switch (dwDI)
	{
	case CCT_DI_CSG_TASK_DATA:
		memcpy(pOutBuf, pInBuf + 25, *(pInBuf + 24));
		*pdwOutLen = *(pInBuf + 24);
		AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_645_DATA, pOutBuf, *pdwOutLen);
		return CCT_ERR_SUCCESS;
		break;

	case CCT_DI_CSG_ERC_DATA:
		//memcpy(pOutBuf, pInBuf + 23, *(pInBuf + 22));
		//*pdwOutLen = *(pInBuf + 22);
		//AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_645_DATA, pOutBuf, *pdwOutLen);
		//return CCT_ERR_SUCCESS;
		break;

	case CCT_DI_CSG_POWERFAIL_DATA:
		GetCsgEvtSendData(pInBuf, wInLen, pOutBuf, pdwOutLen);
		return CCT_ERR_SUCCESS;
		break;

	default:
		TRACE("GetCsgDataFromTaskRepFrm err.\n");
		break;

	}

	return CCT_ERR_SUCCESS;
}

//*****************************************************************************
// 函数名称: GetCsgEvtSendData
// 功能描述: 
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
signed int CCctReader::GetCsgEvtSendData(unsigned char *pInBuf, unsigned short wInLen,
												unsigned char *pOutBuf, unsigned int *pdwOutLen)
{
	TTime tTime;
	unsigned char *p = m_pRx;
	unsigned char bDataPos = 0;

	if ((pInBuf == NULL) || (wInLen == 0) || 
		(pOutBuf == NULL) || (pdwOutLen == 0))
	{
		return (-1);
	}

	
	if (IsCsgAddrRegion(pInBuf, wInLen))
	{
		bDataPos = 26;
	}
	else
	{
		bDataPos = 14;
	}

	GetTime(&tTime);
	unsigned short wDataSize = (pInBuf[2] << 8) + pInBuf[1] - bDataPos -2;
	m_dwOutBufLen = 0;

	for (unsigned short i = 0; i < wDataSize / 6; i++)
	{
		*m_pOutBuf = 1; 	//停电ERC
		m_dwOutBufLen++;
		memcpy(m_pOutBuf + 1, (unsigned char *)p + bDataPos, 6);	//ADDR
		m_dwOutBufLen += 6;
		p += 6;

		memcpy(m_pOutBuf + m_dwOutBufLen, &tTime, sizeof(TTime));
		m_dwOutBufLen += sizeof(TTime);

		//hfg-0120加记录到数据库
		//AddDebugPowerOffData2Db(0x30);


		AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_EVENT_DATA, m_pOutBuf, m_dwOutBufLen);
		TRACEHEX("DoAutoRepFrm PowerOff Event", m_pOutBuf, m_dwOutBufLen);
		m_dwOutBufLen = 0;
	}

	CctCsgConfirm();

	return CCT_ERR_SUCCESS;
}

//*****************************************************************************
// 函数名称: CctCsgConfirm
// 功能描述: 
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
void CCctReader::CctCsgConfirm()
{
	unsigned char bBuf[64];
	unsigned int dwLen = 0;
	memset(bBuf, 0, sizeof(bBuf));
	if (CCT_ERR_SUCCESS == MakeCsgQueryMainNodeFrm(bBuf, &dwLen, 0, 0xe8010101, NULL, 0))
	{
		AppSendMessage2Queue(APP_QUEUE_ID_CCT, APP_MSG_ID_CCT_CON, bBuf, dwLen);
	}
}

//*****************************************************************************
// 函数名称: Make3762ReadFrm
// 功能描述: 
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
signed int CCctReader::MakeCsgQueryMainNodeFrm(unsigned char* pbTxBuf, unsigned int *pdwLen,
																unsigned char bAfn, unsigned int dwFn,
																unsigned char* pbData, unsigned short wDataLen)
{

	if (pbTxBuf == NULL)
	{
		return (-1);
	}

	unsigned char *pbBuf = pbTxBuf;

	//1.头部
	*pbBuf = 0x68;
	pbBuf += 3;

	//2.控制域
	*pbBuf = 0x40;
	pbBuf++;

	//3.源地址域
	//memset(pbBuf, 0, 6);
	//pbBuf += 6;

	//4.目的地址域
	if (pbData != NULL)
	{
		unsigned char bMtrAddr[6] = { 0 };
		unsigned char bNodeAddr[6] = { 0 };
		memcpy(bMtrAddr, GetMtrAddr(pbData), 6);
		memcpy(pbBuf, bMtrAddr, 6);
		pbBuf += 6;
	}

	//5.AFN
	*pbBuf = bAfn;
	pbBuf++;

	//6.SEQ
	*pbBuf = m_bTxSN;
	m_bTxSN++;
	pbBuf++;

	//7.DI
	memcpy(pbBuf, (unsigned char *)&dwFn, 4);
	pbBuf += 4;

	//11.数据长度
	*pbBuf = (unsigned char)wDataLen;
	pbBuf++;

	//12.数据
	memcpy(pbBuf, pbData, wDataLen);
	pbBuf += wDataLen;

	//13.校验
	*pbBuf = CheckSum(pbTxBuf + 3, pbBuf - pbTxBuf - 3);
	pbBuf++;

	//14.尾
	*pbBuf = 0x16;
	pbBuf++;

	//15.填写长度域
	*(pbTxBuf + 1) = (pbBuf - pbTxBuf) % 0xff;
	*(pbTxBuf + 2) = (pbBuf - pbTxBuf) / 0xff;


	*pdwLen = (pbBuf - pbTxBuf);
	return CCT_ERR_SUCCESS;
}

//*****************************************************************************
// 函数名称: Make3762ReadFrm
// 功能描述: 
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
signed int CCctReader::Make3762CsgReadFrm(unsigned char* pbTxBuf, unsigned int *pdwLen,
	unsigned char bAfn, unsigned int dwFn,
	unsigned char* pbData, unsigned short wDataLen)
{

	if (pbTxBuf == NULL)
	{
		return (-1);
	}

	unsigned char *pbBuf = pbTxBuf;

	//1.头部
	*pbBuf = 0x68;
	pbBuf += 3;

	//2.控制域
	*pbBuf = 0x60;
	pbBuf++;

	//3.源地址域
	memset(pbBuf, 0, 6);
	pbBuf += 6;

	//4.目的地址域
	if (pbData != NULL)
	{
		unsigned char bMtrAddr[6] = { 0 };
		unsigned char bNodeAddr[6] = { 0 };
		memcpy(bMtrAddr, GetMtrAddr(pbData), 6);
		memcpy(pbBuf, bMtrAddr, 6);
		pbBuf += 6;
	}

	//5.AFN
	*pbBuf = bAfn;
	pbBuf++;

	//6.SEQ
	*pbBuf = m_bTxSN;
	m_bTxSN++;
	pbBuf++;

	//7.DI
	memcpy(pbBuf, (unsigned char *)&dwFn, 4);
	pbBuf += 4;

	//8.任务号
	memcpy(pbBuf, (unsigned char *)&m_wTaskNo, 2);
	pbBuf += 2;
	m_wTaskNo++;

	//9.任务优先级
	*pbBuf = 0x80;
	pbBuf++;

	//10.超时时长
	memcpy(pbBuf, (unsigned char *)&m_wTaskTimeOut, 2);
	pbBuf += 2;

	//11.数据长度
	*pbBuf = (unsigned char)wDataLen;
	pbBuf++;

	//12.数据
	memcpy(pbBuf, pbData, wDataLen);
	pbBuf += wDataLen;

	//13.校验
	*pbBuf = CheckSum(pbTxBuf + 3, pbBuf - pbTxBuf - 3);
	pbBuf++;

	//14.尾
	*pbBuf = 0x16;
	pbBuf++;

	//15.填写长度域
	*(pbTxBuf + 1) = (pbBuf - pbTxBuf) % 0xff;
	*(pbTxBuf + 2) = (pbBuf - pbTxBuf) / 0xff;


	*pdwLen = (pbBuf - pbTxBuf);
	return CCT_ERR_SUCCESS;
}
///////////////////////////////////////////南网数据处理接口///////////////////////////////////////////

//hfg-0120加记录到数据库
//*****************************************************************************
// 函数名称: AddDebugPowerOffData2Db
// 功能描述: 
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
void AddDebugPowerOffData2Db(char bFlag)
{
	if (NULL != g_pDbMgr)
		g_pDbMgr->CreateTables();

	//unsigned char buf[32];
	//memset(buf,0x30,sizeof(buf));
	//HexToStr((char *)buf, (char *)pOutBuf, pdwOutLen);

	if (NULL != g_pDbMgr)	
		g_pDbMgr->AddAlarmInfo((char *)&bFlag, 1);

}

