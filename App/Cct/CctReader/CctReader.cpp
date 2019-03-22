//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : CctReader.cpp
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : ����
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
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
// ��������: Init
// ��������: 
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
bool CCctReader::Init(TCctPara *pCctPara)
{
	m_fStopRdMtr = false;
	m_fStopRouter = false;
	m_fInReadPeriod = true;	//�Ƿ��ڳ���ʱ����
	m_fSchMtr = false;		//��������״̬
	m_fInInterVRead = false;
	m_fDirectOpera = false;	//����ֱ�Ӳ���
	m_wRdTurn = 0;			//�ֳ�����
	m_dwStudyClick = 0;		//ѧϰʱ��
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

	m_bRxSN = 0;			//���ͱ������к�
	m_bTxSN = 0;			//���ܱ������к�

	m_wTaskNo = 1;			//���������			
	m_wTaskTimeOut = 60;	//������ʱʱ��
	memset(m_bMainAddr,0xee, sizeof(m_bMainAddr));
	m_fIsMainNodeAddrOk = false;
	m_dwUartPulseClick = GetClick();

	//hfg-0120�Ӽ�¼�����ݿ�
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
// ��������: Run
// ��������: 
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
	//ע��������ʱ��
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_CCT;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_UART_PULSE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_CCT;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE;
	//�����������
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
// ��������: CctReaderMsgProc
// ��������: 
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
signed int CCctReader::CctReaderMsgProc(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize)
{

	if ((NULL == pFrmData) || (0 == dwFrmSize))
	{
		return (-1);
	}

	switch (dwMsgId)
	{
		//��վ�����ĳ�������
	case APP_MSG_ID_SVR_REQ:
	case APP_MSG_ID_DB_REQ:
		//TRACE("CctReaderMsgProc send APP_MSG_ID_SVR_REP to APP_QUEUE_ID_DRV.\n");
		//������·�� AFN=1301
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
// ��������: CctReaderTaskUartPulse
// ��������: 
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
void CCctReader::CctReaderTaskUartPulse()
{
	//unsigned char bPulseFrm[] = {0x68,0x0f,0x00,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x08,0x00,0x4e,0x16};
	unsigned char bPulseFrm[] = {0x68,0x0f,0x00,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x47,0x16};		//ȷ��/����/����-��չF3
	
	if (CCT_ERR_SUCCESS == Send(bPulseFrm, sizeof(bPulseFrm)))
	{
		unsigned int dwClick = GetClick();
		AppSendMessage2Queue(APP_QUEUE_ID_CCTRCV, APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE, (unsigned char *)&dwClick, sizeof(unsigned int));
	}
}


//*****************************************************************************
// ��������: CctReaderTaskKeepAlive
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
void CCctReader::CctReaderTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

//*****************************************************************************
// ��������: CctRecvTaskKeepAlive
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
void CCctReader::CctRecvTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

//*****************************************************************************
// ��������: GetCctReaderCcoTop
// ��������: ��ѯCCo���˽ṹ����
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
// ��������: DoCctRcvFrm
// ��������: 
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
					//·�ɲ���
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

	//����������������δ�յ����ڷ��͵�����
	if (dwClick > m_dwUartPulseClick + TASK_UART_PULSE_CHECK_TIME *2)
	{
		//SystemReboot();
	}

}

//*****************************************************************************
// ��������: DoRouterConFrm
// ��������: 
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

	//��������
	if (wFn == FN3)
	{
		m_dwUartPulseClick = GetClick();
	}

}

//*****************************************************************************
// ��������: DoDispatchParaFrm
// ��������: 
//
// ����˵��: ·�ɲ��� AFN=03 FN=04
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
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
// ��������: DoAutoRepFrm
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	  ��: sUesTcxF
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

	//�ϱ�ͣ���¼�
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
			*m_pOutBuf = 1;		//ͣ��ERC
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
// ��������: CctConfirm
// ��������: 
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
// ��������: DoAutoRepFrm
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	  ��: sUesTcxF
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
// ��������: IsAddrRegion
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
//*****************************************************************************
bool CCctReader::IsAddrRegion(unsigned char *pBuf, unsigned short wLen)
{
	if (0 != (*(pBuf + 4) & 0x04))
		return true;
	else
		return false;
}

//*****************************************************************************
// ��������: GetAfn
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
//*****************************************************************************
unsigned char CCctReader::GetAfn(unsigned char *pBuf, unsigned short wLen)
{
	if (IsAddrRegion(pBuf, wLen))
	{
		//����ַ��
		return *(pBuf + 22);

	}
	else
	{
		//������ַ��
		return *(pBuf + 10);
	}

	return 0;
}

#define FRM_645_DATA_POS			29
//*****************************************************************************
// ��������: DoDispatchFrm
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
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
// ��������: GetMainAddr
// ��������: 
//
// ����˵��: ��ȡ���ڵ��ַ
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
//*****************************************************************************
unsigned char* CCctReader::GetMainAddr()
{
	return m_bMainAddr;
}

//*****************************************************************************
// ��������: ReadMainAddr
// ��������: 
//
// ����˵��: ��ȡ���ڵ��ַ
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
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
// ��������: Get645FrmSizeFrom3762Frm
// ��������: 
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
// ��������: DoCctUpdate
// ��������: ״̬����
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
void CCctReader::DoCctUpdate()
{
	signed int iRet;

	LockReader();
	//ֻ�Ա�����������Ч,�����Զ�·���㷨��485������������������Ч
	//���ڱ�������ģ����˵,��������ģ������һ֡,���ն˻ش�һ֡,
	//���԰�������������Ҳ����Ӱ��ϵͳ��ʵʱ�������ң���������������Ӧʱ��.
	//���Ƕ�����������ģ����˵,һ�㶼��һ�����ܺ��������е����һ��,����
	//ִ��ʱ�䶼�Ƚϳ�,����ֻ�ܰѳ���������ڲ��ֽ��оֲ�����,�Ա���ʵʱ����
	//�Ե��Ĳ���

	iRet = UpdateCctReader();	//����·��������

	switch (m_bState)
	{
	case ROUTER_STA_LEARN_ALL:
		iRet = DoLearnAll();
		break;

	case ROUTER_STA_LEARN_LOCAL:
		iRet = DoLearnLocal();
		break;

	case ROUTER_STA_READ:
		//������·�� AFN=1401
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

//����(ͨ����)·�ɳ��� XC��RSC��HISI AFN=13 FN=01
//ͨ����·�ɲ��ô���ɼ����ڵ��һ�Զ����⣬
//ֱ�ӵ���ַ�Ӳɼ�����ַ��֡�·����������ɼ�����һ�Զ����ѯ
//*****************************************************************************
// ��������: DoCctReader
// ��������: 
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

	//���� ֹͣ����״̬ || ���ڳ���ʱ���� ��⵽ģ�鱻������
	if (m_fStopRdMtr || !IsInReadPeriod() || m_fSchMtr || m_fPlcMoudleChange)
	{
		return;
	}

	//StopRouter();

	//bBuf-����645,dwBufLen
	m_dwOutBufLen = 0;
	dwRet = CctReadItemData(pFrmData, dwFrmSize, m_pOutBuf, &m_dwOutBufLen);

	//���Դ���
	//unsigned char bdata[] = {0x68, 0x01, 0x00, 0x00, 0x09, 0x17, 0x20, 0x68, 0x91, 0x08, 0x33, 0x33, 0x34, 0x33, 0x57, 0x5a, 0x33, 0x33, 0x8e, 0x16};
	//AppSendMessage2Queue(APP_QUEUE_ID_DB, APP_MSG_ID_SVR_REP, bdata, sizeof(bdata));

	//ResumeRouter();

}

//*****************************************************************************
// ��������: StopRouter
// ��������: 
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
// ��������: ResumeRouter
// ��������: �ָ�·�ɳ���
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
// ��������: SetCctPara
// ��������: 
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
void CCctReader::SetCctPara(TCctPara *pCctPara)
{
	m_pCctPara = pCctPara;
}

//*****************************************************************************
// ��������: CtrlRoute
// ��������: ·�ɿ���
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
bool CCctReader::CtrlRoute(unsigned short wFn)
{
	MakeCtrlFrm(m_pTx, &m_TxLen, CCT_AFN_3762_CTRLRT, wFn);
	Send(m_pTx, m_TxLen);
	return true;
}

//*****************************************************************************
// ��������: MakeCtrlFrm
// ��������: 
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
bool CCctReader::MakeCtrlFrm(unsigned char *pBuf, unsigned int *pdwLen, unsigned char bAFN, unsigned short wFn)
{
	unsigned char bLenBytes = m_pCctPara->wFrmLenBytes - 1;	//֡�����ֽ���	
	unsigned char bAddrLen = 12;							//���ڵ��ַ����
	unsigned char bPRM = FRM_C_PRM_1;						//������
	unsigned char bMode = m_pCctPara->bLink;
	unsigned short wLen = 15;

	pBuf[0] = 0x68;
	pBuf[1] = (unsigned char)wLen;							//����L
	pBuf[2] = (unsigned char)(wLen >> 8);					//����L
	pBuf[2 + bLenBytes] = (unsigned char)(bPRM*(0x01 << 6) + bMode);
	pBuf[3 + bLenBytes] = 0;		//D2ͨ��ģ���ʶ��0��ʾ�Լ�������ͨ��ģ�����;1��ʾ���ز����ͨ��ģ�������
	pBuf[4 + bLenBytes] = 0;								//�ŵ�
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
// ��������: MakeCtrlFrm
// ��������: 
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
bool CCctReader::Make3762Frm(unsigned char *pBuf, unsigned int *pdwLen, unsigned char bAFN, unsigned short wFn, unsigned char *pInBuf, unsigned int dwLen)
{
	unsigned char bLenBytes = m_pCctPara->wFrmLenBytes - 1;	//֡�����ֽ���	
	unsigned char bAddrLen = 12;							//���ڵ��ַ����
	unsigned char bPRM = FRM_C_PRM_1;						//������
	unsigned char bMode = m_pCctPara->bLink;
	unsigned short wLen = 15 + dwLen;

	pBuf[0] = 0x68;
	pBuf[1] = (unsigned char)wLen;							//����L
	pBuf[2] = (unsigned char)(wLen >> 8);					//����L
	pBuf[2 + bLenBytes] = (unsigned char)(bPRM*(0x01 << 6) + bMode);
	pBuf[3 + bLenBytes] = 0;		//D2ͨ��ģ���ʶ��0��ʾ�Լ�������ͨ��ģ�����;1��ʾ���ز����ͨ��ģ�������
	pBuf[4 + bLenBytes] = 0;								//�ŵ�
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
// ��������: Make3762ReadFrm
// ��������: 
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
signed int CCctReader::Make3762ReadFrm(unsigned char* pbTxBuf, unsigned int *pdwLen,
	unsigned char bAfn, unsigned short wFn,
	unsigned char* pbData, unsigned short wDataLen)
{
	unsigned short wLen = 0;
	unsigned char bAddrLen = 12;							//���ڵ��ַ����
	unsigned char bLenBytes = m_pCctPara->wFrmLenBytes - 1;	//֡�����ֽ���	
	unsigned short wAppDataFieldLen = 0;
	unsigned char bPRM = FRM_C_PRM_1;						//������
	unsigned char bCS = 0;

	if (pbData == NULL && wDataLen != 0)
	{
		TRACE("CCctReader::MakeFrm: fail.\n");
		return 0;
	}

	//1.ͷ��
	pbTxBuf[0] = 0x68;

	//2.��Ϣ��R
	pbTxBuf[3 + bLenBytes] = (bAddrLen > 0) ? (1 << 2) : 0;		//D2ͨ��ģ���ʶ��0��ʾ�Լ�������ͨ��ģ�����;1��ʾ���ز����ͨ��ģ�������
	//pbTxBuf[4 + bLenBytes] = bCn;								//�ŵ�
	pbTxBuf[4 + bLenBytes] = 0;								//�ŵ�
	pbTxBuf[5 + bLenBytes] = 0;
	pbTxBuf[6 + bLenBytes] = 0;
	pbTxBuf[7 + bLenBytes] = 0;
	pbTxBuf[8 + bLenBytes] = 0;

	//3.Э������
	//02 00 00 10   68 07 07 00 00 34 12 68 11 04 34 34 39 38 12 16
	unsigned char bData[100];
	memset(bData, 0, sizeof(bData));
	bData[0] = 2;											//���Э������97/07
	bData[1] = 0;											//ͨ����ʱ����Ա�־
	bData[2] = 0;											//�ӽڵ㸽���ڵ�����
	bData[3] = (unsigned char)wDataLen;						//Э�����ݳ�(645֡��)
	memcpy(bData + 4, pbData, wDataLen);

	//4.�ڵ��ַ
	//Ŀ�Ľڵ��ַ�������ַ�ʽ 1.Ŀ�Ľڵ���д�ɼ�����ַ 2.Ŀ�Ľڵ���д����ַ, ���õ�2��ʽ
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

	//��Ӧ����
	wAppDataFieldLen = MakeAppDataField(bAfn, wFn, bData, bData[3] + 4, &pbTxBuf[9 + bAddrLen + bLenBytes]);

	wLen = 6 + bAddrLen + wAppDataFieldLen + 5 + bLenBytes;
	pbTxBuf[1] = (unsigned char)wLen;						//����L
	if (bLenBytes == 1)
	{
		pbTxBuf[2] = (unsigned char)(wLen >> 8);			//����L
	}

	//ģ������
	unsigned char bMode = m_pCctPara->bLink;
	//������
	pbTxBuf[2 + bLenBytes] = (unsigned char)(bPRM*(0x01 << 6) + bMode); //������C

	//376.2֡���
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

	//����У��
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
// ��������: IsNeedSeq
// ��������: ֡���
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
// ��������: SetAddrField
// ��������: 
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
void CCctReader::SetAddrField(unsigned char *pbTxBuf, unsigned char *pbAddr)
{
	unsigned char bAddr[6] = { 0 };
	//GetTerminalAddr(bAddr);
	memset(bAddr, 0, sizeof(bAddr));		//���ڵ��ַ��ʱ��д0

	memcpy(&pbTxBuf[FRM_ADDR], bAddr, 6);
	memcpy(&pbTxBuf[FRM_ADDR + 6], pbAddr, 6);
}

//*****************************************************************************
// ��������: GetAcqAddr
// ��������: �õ���ַ��ɼ�����ַ
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
void CCctReader::GetAcqAddr(unsigned char *pMtrAddr, unsigned char *pAcqAddr)
{
	pAcqAddr = pMtrAddr;
}

//*****************************************************************************
// ��������: FnToDt
// ��������: 
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
void CCctReader::FnToDt(unsigned short wFn, unsigned char* pbDt)
{
	pbDt[0] = 0x1 << ((wFn - 1) % 8);
	pbDt[1] = (unsigned char)((wFn - 1) / 8);
}

//*****************************************************************************
// ��������: DtToFn
// ��������: 
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


//����:��֡Ӧ��������
//*****************************************************************************
// ��������: MakeAppDataField
// ��������: 
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
// ��������: RcvFrm
// ��������: 
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
#define FSM_STATUS_RX_BGEIN 		(0)
#define FSM_STATUS_RX_FRMHEAD		(1)
#define FSM_STATUS_RX_FRMLEN		(2)
#define FSM_STATUS_RX_FRMDATA		(3)
#define FRM_INF		        		(4) 	  //��Ϣ��R
#define RX_DATA_MAX_LEN				(2048)
bool CCctReader::RcvFrm(unsigned char *pRx, unsigned int *pRxLen)
{
	signed int dwRcvLen = 0;	//ʵ�ʽ��ճ���
	unsigned int dwRcvSize = 0;	//Ԥ�ƽ������ݳ���
	unsigned int dwLen = 0;		//�ܼƽ������ݳ���
	unsigned int dwDataRegionLen = 0; //�����򳤶�
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
				//��ʣ�µ�����
				dwRcvSize = dwLastLen;
				bStatus = FSM_STATUS_RX_FRMLEN;
			}

			break;

		case FSM_STATUS_RX_FRMLEN:
			if (bRcvDataCount == 0)
			{
				dwDataRegionLen = (pRx[2] << 8) + pRx[1] - 3; 					//��ȥ�������ͷ3���ֽ�
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
				//��ʣ�µ�����
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
				//hfg-0120�Ӽ�¼�����ݿ�
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
					//hfg-0120�Ӽ�¼�����ݿ�
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
					//hfg-0120�Ӽ�¼�����ݿ�
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
// ��������: Send
// ��������: 
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


//�������ݳ���
//*****************************************************************************
// ��������: DoPointRead
// ��������: 
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
signed int CCctReader::DoPointRead()
{
	//�����������ݳ���
	DoMinPointRead();

	//Сʱ�������ݳ���
	DoHourPointRead();

	return 0;
}

//*****************************************************************************
// ��������: DoMinPointRead
// ��������: ����������������
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
signed int CCctReader::DoMinPointRead()
{
	return 0;
}

//*****************************************************************************
// ��������: DoHourPointRead
// ��������: ����Сʱ��������
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
signed int CCctReader::DoHourPointRead()
{
	return 0;
}


///////////////////////////////////////////�������ݴ���ӿ�///////////////////////////////////////////
//*****************************************************************************
// ��������: DoCsgCctRcvFrm
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	  ��: sUesTcxF
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
// ��������: IsCsgAddrRegion
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
//*****************************************************************************
bool CCctReader::IsCsgAddrRegion(unsigned char *pBuf, unsigned short wLen)
{
	if (0 != (*(pBuf + 3) & 0x40))
		return true;
	else
		return false;
}

//*****************************************************************************
// ��������: GetCsgAfn
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
//*****************************************************************************
unsigned char CCctReader::GetCsgAfn(unsigned char *pBuf, unsigned short wLen)
{
	if (IsCsgAddrRegion(pBuf, wLen))
	{
		//����ַ��
		memcpy(m_bMainAddr, pBuf+10, sizeof(m_bMainAddr));
		return *(pBuf + 16);

	}
	else
	{
		//������ַ��
		return *(pBuf + 4);
	}
}

//*****************************************************************************
// ��������: GetCsgDI
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
//*****************************************************************************
unsigned int CCctReader::GetCsgDI(unsigned char *pBuf, unsigned short wLen)
{
	if (IsCsgAddrRegion(pBuf, wLen))
	{
		//����ַ��
		return *(unsigned int *)(pBuf + 18);

	}
	else
	{
		//������ַ��
		return *(unsigned int *)(pBuf + 6);
	}
}

//*****************************************************************************
// ��������: ReadCsgMainAddr
// ��������: 
//
// ����˵��: ��ȡ���ڵ��ַ
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
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
// ��������: DoDispatchCsgParaFrm
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
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
// ��������: DoCsgDispatchParaFrm
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
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
// ��������: GetCsgDataFromTaskRepFrm
// ��������: 
//
// ����˵��:
//	This function has no arguments.
//
// �� �� ֵ:
//	  ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��	 ��: sUesTcxF
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
// ��������: GetCsgEvtSendData
// ��������: 
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
		*m_pOutBuf = 1; 	//ͣ��ERC
		m_dwOutBufLen++;
		memcpy(m_pOutBuf + 1, (unsigned char *)p + bDataPos, 6);	//ADDR
		m_dwOutBufLen += 6;
		p += 6;

		memcpy(m_pOutBuf + m_dwOutBufLen, &tTime, sizeof(TTime));
		m_dwOutBufLen += sizeof(TTime);

		//hfg-0120�Ӽ�¼�����ݿ�
		//AddDebugPowerOffData2Db(0x30);


		AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_EVENT_DATA, m_pOutBuf, m_dwOutBufLen);
		TRACEHEX("DoAutoRepFrm PowerOff Event", m_pOutBuf, m_dwOutBufLen);
		m_dwOutBufLen = 0;
	}

	CctCsgConfirm();

	return CCT_ERR_SUCCESS;
}

//*****************************************************************************
// ��������: CctCsgConfirm
// ��������: 
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
// ��������: Make3762ReadFrm
// ��������: 
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
signed int CCctReader::MakeCsgQueryMainNodeFrm(unsigned char* pbTxBuf, unsigned int *pdwLen,
																unsigned char bAfn, unsigned int dwFn,
																unsigned char* pbData, unsigned short wDataLen)
{

	if (pbTxBuf == NULL)
	{
		return (-1);
	}

	unsigned char *pbBuf = pbTxBuf;

	//1.ͷ��
	*pbBuf = 0x68;
	pbBuf += 3;

	//2.������
	*pbBuf = 0x40;
	pbBuf++;

	//3.Դ��ַ��
	//memset(pbBuf, 0, 6);
	//pbBuf += 6;

	//4.Ŀ�ĵ�ַ��
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

	//11.���ݳ���
	*pbBuf = (unsigned char)wDataLen;
	pbBuf++;

	//12.����
	memcpy(pbBuf, pbData, wDataLen);
	pbBuf += wDataLen;

	//13.У��
	*pbBuf = CheckSum(pbTxBuf + 3, pbBuf - pbTxBuf - 3);
	pbBuf++;

	//14.β
	*pbBuf = 0x16;
	pbBuf++;

	//15.��д������
	*(pbTxBuf + 1) = (pbBuf - pbTxBuf) % 0xff;
	*(pbTxBuf + 2) = (pbBuf - pbTxBuf) / 0xff;


	*pdwLen = (pbBuf - pbTxBuf);
	return CCT_ERR_SUCCESS;
}

//*****************************************************************************
// ��������: Make3762ReadFrm
// ��������: 
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
signed int CCctReader::Make3762CsgReadFrm(unsigned char* pbTxBuf, unsigned int *pdwLen,
	unsigned char bAfn, unsigned int dwFn,
	unsigned char* pbData, unsigned short wDataLen)
{

	if (pbTxBuf == NULL)
	{
		return (-1);
	}

	unsigned char *pbBuf = pbTxBuf;

	//1.ͷ��
	*pbBuf = 0x68;
	pbBuf += 3;

	//2.������
	*pbBuf = 0x60;
	pbBuf++;

	//3.Դ��ַ��
	memset(pbBuf, 0, 6);
	pbBuf += 6;

	//4.Ŀ�ĵ�ַ��
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

	//8.�����
	memcpy(pbBuf, (unsigned char *)&m_wTaskNo, 2);
	pbBuf += 2;
	m_wTaskNo++;

	//9.�������ȼ�
	*pbBuf = 0x80;
	pbBuf++;

	//10.��ʱʱ��
	memcpy(pbBuf, (unsigned char *)&m_wTaskTimeOut, 2);
	pbBuf += 2;

	//11.���ݳ���
	*pbBuf = (unsigned char)wDataLen;
	pbBuf++;

	//12.����
	memcpy(pbBuf, pbData, wDataLen);
	pbBuf += wDataLen;

	//13.У��
	*pbBuf = CheckSum(pbTxBuf + 3, pbBuf - pbTxBuf - 3);
	pbBuf++;

	//14.β
	*pbBuf = 0x16;
	pbBuf++;

	//15.��д������
	*(pbTxBuf + 1) = (pbBuf - pbTxBuf) % 0xff;
	*(pbTxBuf + 2) = (pbBuf - pbTxBuf) / 0xff;


	*pdwLen = (pbBuf - pbTxBuf);
	return CCT_ERR_SUCCESS;
}
///////////////////////////////////////////�������ݴ���ӿ�///////////////////////////////////////////

//hfg-0120�Ӽ�¼�����ݿ�
//*****************************************************************************
// ��������: AddDebugPowerOffData2Db
// ��������: 
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

