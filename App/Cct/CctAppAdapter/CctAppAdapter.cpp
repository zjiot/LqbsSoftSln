//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : CctAppAdapter.cpp
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : CctAppҵ��ģ��������
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************
#include "CctAppAdapter.h"
#include "CctMain.h"
#include "CctCfg.h"

CCT_APP_INFO g_AppVersion = 
{
	{'L','Q','B','S','_','C','C','T','_','A','P','P','\0'},
	{2019,2,20,14,0,0}
};

CCctAppAdapter::CCctAppAdapter()
{
	memset(m_bTmAddr,0xee, sizeof(m_bTmAddr));
}

CCctAppAdapter::~CCctAppAdapter()
{

}

//*****************************************************************************
// ��������: Init
// ��������: ע��Эӳ������
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
bool CCctAppAdapter::Init()
{	
	m_CmdFnMap.clear();
	//RegisterAppAdapter(CCT_APP_ID);

	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN0, (DOCMDFN)&CCctAppAdapter::CctAppLogin);
	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN1, (DOCMDFN)&CCctAppAdapter::CctAppHeartBeat);
	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN2, (DOCMDFN)&CCctAppAdapter::CctAppSetMtrPara);
	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN3, (DOCMDFN)&CCctAppAdapter::CctReadAllMtrData);
	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN4, (DOCMDFN)&CCctAppAdapter::CctReadOneMtrData);
	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN5, (DOCMDFN)&CCctAppAdapter::CctGetEvetMtrData);
	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN6, (DOCMDFN)&CCctAppAdapter::CctGetCcoTop);
	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN7, (DOCMDFN)&CCctAppAdapter::CctAppSoftUpgrade);
	AddCmdFn((unsigned short)ADAPTER_CCT_APP_FN8, (DOCMDFN)&CCctAppAdapter::CctAppGetVersion);


	//���FN�Ͷ�Ӧ�Ĵ�����......

	TSocketPara tSocketPara;
	LoadSocketPara(&tSocketPara);
	m_pIf = new CSocketIf();
	if (m_pIf == NULL) return false;
	m_pIf->Init(tSocketPara.bIP, tSocketPara.dwPort);

	m_fAdapterConnected = false;
	ConnectAdapter();
	
	return true;
}

//*****************************************************************************
// ��������: AddCmd
// ��������: ע��Эӳ������
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
void CCctAppAdapter::AddCmdFn(unsigned short wCmdCode, DOCMDFN pFunc)
{
	m_CmdFnMap.insert(ADAPTER_CMDFN_PAIR(wCmdCode, pFunc));
}

//*****************************************************************************
// ��������: AddCmd
// ��������: ɾ��Эӳ������
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
void CCctAppAdapter::DeleteCmd(unsigned short wCmdCode)
{
	ADAPTER_CDMFN_ITER iter = m_CmdFnMap.find(wCmdCode);
	
	if (iter != m_CmdFnMap.end()) 
	{
		m_CmdFnMap.erase(iter);
	}
}

//*****************************************************************************
// ��������: GetCmdCode
// ��������: ��ȡ������
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
unsigned short CCctAppAdapter::GetCmdCode(unsigned char *pBuf)
{
	return (unsigned short)(*(pBuf + POS_CMDFN_CODE));
}

//*****************************************************************************
// ��������: DispatchPackage
// ��������: ӳ��
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
signed int CCctAppAdapter::DispatchPackage(unsigned char *pInBuf, unsigned char *pOutBuf)
{

	if (!CheckTmAddr(pInBuf))
	{
		TRACE("DispatchPackage CheckTmAddr err !.\n");
		return (-1);
	}

	unsigned short wCmdCode = GetCmdCode(pInBuf);
	if (ADAPTER_CCT_APP_FN_MAX != wCmdCode)
	{
		ADAPTER_CDMFN_ITER iter = m_CmdFnMap.find(wCmdCode);
		
		if (iter != m_CmdFnMap.end())
		{
			return (this->*(iter->second))(pInBuf, pOutBuf);
		}
	}

	return (-1);
}

//*****************************************************************************
// ��������: CheckTmAddr
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
bool CCctAppAdapter::CheckTmAddr(unsigned char *pInBuf)
{
	unsigned char bAddr[TERMINAL_ADDR_SIZE];
	memset(bAddr, 0, TERMINAL_ADDR_SIZE);
	
	if (true == GetTerminalAddr(bAddr))
	{
		if (0 == memcmp(bAddr, pInBuf + POS_TMADDR_POS, TERMINAL_ADDR_SIZE))
		{
			return true;
		}
	}

	return false;
}


//*****************************************************************************
// ��������: AdapterDoPackage
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
signed int CCctAppAdapter::AdapterDoPackage(ADAPTER_CCT_APP_FN_ENUM wCmdCode, 
													unsigned char *pRxBuf, unsigned short dwRxLen, 
													unsigned char *pTxBuf, unsigned short dwTxLen)
{
	if (true == MakeAdapterFrm(wCmdCode, pRxBuf, dwRxLen, pTxBuf, &dwTxLen))
	{
		//return AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pTxBuf, dwTxLen);
		return AdapterSendPackage2Svr(pTxBuf, dwTxLen);
	}

	return (-1);
}


//*****************************************************************************
// ��������: SetPara
// ��������: ���ò���
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
signed int CCctAppAdapter::CctAppSetMtrPara(unsigned char *pInBuf, unsigned char *pOutBuf)
{
	if (pInBuf == NULL)return (-1);

	TRACEHEX("CctAppSetMtrPara", pInBuf, (unsigned short)(*(pInBuf+1))+3);
#if 0
	TMtrArg *ptMtrArg = new [sizeof(TMtrArg)*MAX_POINT_NUM];
	if (ptMtrArg == NULL) return (-1);

	if (true != GetAppData(pInBuf, pOutBuf)) return (-1);

	unsigned short wMtrNum = (unsigned short)(*pOutBuf);
	pOutBuf += 2;
#endif

	return 0;
}




//*****************************************************************************
// ��������: CctReadOneMtrData
// ��������: ��ȡһ�����һ��ID������
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
signed int CCctAppAdapter::CctReadOneMtrData(unsigned char *pInBuf, unsigned char *pOutBuf)
{
	unsigned short wOutBufLen = MAX_ADAPTER_FRM_LEN;

	if ((pInBuf == NULL) || (pOutBuf == NULL)) return (-1);

	TRACEHEX("CctReadOneMtrData", pInBuf, (unsigned short)(*(pInBuf+1))+3);

	if ( true == Get645FrmFromBuf(pInBuf, (unsigned short)*(pInBuf + 1) + 3, pOutBuf, &wOutBufLen))
	{
		AppSendMessage2Queue(APP_QUEUE_ID_DB, APP_MSG_ID_SVR_REQ, pOutBuf, wOutBufLen);
	}
	
	return 0;
}


//*****************************************************************************
// ��������: CctReadAllMtrData
// ��������: ��ȡ���е��һ��ID������
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
signed int CCctAppAdapter::CctReadAllMtrData(unsigned char *pInBuf, unsigned char *pOutBuf)
{
	return 0;
}

//*****************************************************************************
// ��������: Login
// ��������: APP��½
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
bool CCctAppAdapter::Login()
{
	unsigned short wOutBufLen = 0;
	
	unsigned char pOutBuf[64] = {0};
	memset(pOutBuf, 0, sizeof(pOutBuf));

	if (IsBufferAllEE(m_bTmAddr, sizeof(m_bTmAddr)))
	{
		//δ��ȡ���ն˵�ַ
		//return false;
	}
	
	if (true == MakeAdapterFrm(ADAPTER_CCT_APP_FN0, NULL, 0, pOutBuf, &wOutBufLen))
	{
		TRACE("CCctAppAdapter::Login.\n");
		AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pOutBuf, wOutBufLen);		
		return true;
	}

	return false;
	
}


//*****************************************************************************
// ��������: HeartBeat
// ��������: APP����
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
bool CCctAppAdapter::HeartBeat()
{
	unsigned short wOutBufLen = 0;
	
	unsigned char pOutBuf[64] = {0};
	memset(pOutBuf, 0, sizeof(pOutBuf));
	
	if (true == MakeAdapterFrm(ADAPTER_CCT_APP_FN1, 0, NULL, pOutBuf, &wOutBufLen))
	{
		//ֱ���͸������߳�
		//AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pOutBuf, wOutBufLen);
		AdapterSendPackage2Svr(pOutBuf, wOutBufLen);
	}
	
	return 0;
}

//*****************************************************************************
// ��������: CctAppLogin
// ��������: ��������½����
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
signed int CCctAppAdapter::CctAppLogin(unsigned char *pInBuf, unsigned char *pOutBuf)
{
	unsigned short wOutBufLen = 0;

	//if (true == MakeAdapterFrm(ADAPTER_CCT_APP_FN0, pInBuf, pOutBuf, &wOutBufLen))
	{
		//memcpy(pInBuf, pOutBuf, (unsigned short)(*(pInBuf + 1)) + 3);
		//AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pInBuf, wOutBufLen);
	}
	
	return 0;
}

//*****************************************************************************
// ��������: CctAppHeartBeat
// ��������: ��������������
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
signed int CCctAppAdapter::CctAppHeartBeat(ADAPTER_CCT_APP_FN_ENUM wCmdCode, 
													unsigned char *pRxBuf, unsigned short dwRxLen, 
													unsigned char *pTxBuf, unsigned short dwTxLen)
{
	unsigned short wOutBufLen = 0;
	TRACE("CCctAppAdapter::CctAppHeartBeat.\n");

	if (true == MakeAdapterFrm(ADAPTER_CCT_APP_FN1, NULL, 0, pTxBuf, &wOutBufLen))
	{
		//AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pTxBuf, wOutBufLen);
		AdapterSendPackage2Svr(pTxBuf, wOutBufLen);
	}
	
	return 0;
}

//*****************************************************************************
// ��������: SetTime
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
signed int CCctAppAdapter::SetTime(unsigned char *pInBuf, unsigned char *pOutBuf)
{
	TTime *pTime;

	if (NULL == pInBuf)
	{
		return (-1);
	}

	pTime = (TTime *)pInBuf;

	return 0;
}

//*****************************************************************************
// ��������: SetTime
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
signed int CCctAppAdapter::CctGetCcoTop(unsigned char *pInBuf, unsigned char *pOutBuf)
{
	return AppSendMessage2Queue(APP_QUEUE_ID_CCT, APP_MSG_ID_CCT_TOP_REQ, pInBuf, 1);
}

//*****************************************************************************
// ��������: ConnectAdapter
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
bool CCctAppAdapter::ConnectAdapter()
{
	if (m_pIf->IsConnected())
	{
		if (true == Login())
		{
			m_fAdapterConnected = true;
			
			Sleep(1000);
			CctAppGetVersion();
			
			return true;
		}
	}
	else
	{
		m_pIf->SocketReConnect();
		if (m_pIf->IsConnected())
		{
			if (true == Login())
			{
				m_fAdapterConnected = true;
				return true;
			}
		}
	}

	return false;
}

//*****************************************************************************
// ��������: IsAdapterConnected
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
bool CCctAppAdapter::IsAdapterConnected()
{
	return m_fAdapterConnected;
}

//*****************************************************************************
// ��������: MakeAdapterFrm
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
bool CCctAppAdapter::MakeAdapterFrm(unsigned short wCmdCode, unsigned char *pInBuf, 
								unsigned short wInBufLen, unsigned char *pOutBuf, 
								unsigned short *pwOutBufLen)
{	
	unsigned char bTerminalAddr[TERMINAL_ADDR_SIZE] = {0};

	unsigned char *p = pOutBuf;
	
	*p = 0x68;										//ͷ
	p++;
	p += ADAPTER_FRM_LEN;							//Ԥ�������������д

	if (!GetTerminalAddr(bTerminalAddr))
	{
		return false;
	}


	memcpy(p, &bTerminalAddr, TERMINAL_ADDR_SIZE);//�ն˵�ַ
	p += TERMINAL_ADDR_SIZE;

	*p = 0x1;										//������,��ʱ��д1
	p++;
	memcpy(p, (unsigned char *)&wCmdCode, ADAPTER_CMDOCDE_LEN);		//FN
	p += ADAPTER_CMDOCDE_LEN;

	if (( 0 != wInBufLen) && (pInBuf != NULL))		//������
	{
		memcpy(p, pInBuf, wInBufLen);
		p += wInBufLen;
	}

	// +2�ֽ�:1Byte CS 1Byte������0x16
	*pwOutBufLen = p - pOutBuf + 2;		
	unsigned short wLen = *pwOutBufLen - 3;

	//����֡�����ֽ� -3  1Byte-0x68 2Byte-������
	memcpy(pOutBuf + 1, (unsigned char *)&(wLen), ADAPTER_FRM_LEN);
	*p = CheckSum(pOutBuf, *pwOutBufLen - 2);
	p++;

	//������
	*p = 0x16;

	return true;
}

//*****************************************************************************
// ��������: MakeAdapterFrm
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
bool CCctAppAdapter::Get645FrmFromBuf(unsigned char *pInBuf, unsigned short wInBufLen, 
											unsigned char *pOutBuf, unsigned short *pwOutBufLen)
{
	if ((pInBuf == NULL) || (pOutBuf == NULL) || 
		(pwOutBufLen == NULL) || (wInBufLen == 0))
	{
		return false;
	}

	if (wInBufLen < MIN_ADAPTER_LEN)
	{
		return false;
	}

	//12Byte = 0x68(1Byte)+L(2Byte)+TerminalAddr(4Byte)+C(1Byte)+FN(2Byte)+CS(1Byte)+0x16(1Byte)
	*pwOutBufLen = wInBufLen - 14;		
	memcpy(pOutBuf, pInBuf + POS_645FRM_POS, *pwOutBufLen);

	return true;


}

//*****************************************************************************
// ��������: GetAppData
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
bool CCctAppAdapter::GetAppData(unsigned char *pInBuf, unsigned char *pOutBuf)
{
	if ((pInBuf == NULL) || (pOutBuf == NULL))
	{
		return false;
	}

	//12Byte = 0x68(1Byte)+L(2Byte)+TerminalAddr(4Byte)+C(1Byte)+FN(2Byte)+CS(1Byte)+0x16(1Byte)
	pOutBuf = pInBuf + POS_645FRM_POS;
	return true;

}

//*****************************************************************************
// ��������: CctGetEvetMtrData
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
void CCctAppAdapter::CctGetEvetMtrData()
{
	;
}


#define FSM_STATUS_RX_BGEIN 		(0)
#define FSM_STATUS_RX_FRMHEAD		(1)
#define FSM_STATUS_RX_FRMLEN		(2)
#define FSM_STATUS_RX_FRMDATA		(3)

#define RX_DATA_MAX_LEN		(2048)

//*****************************************************************************
// ��������: RcvFrm
// ��������: Э���������֡
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
bool CCctAppAdapter::RcvFrm(unsigned char *pBuf, unsigned int *pdwLen)
{
	signed int dwRcvLen = 0;	//ʵ�ʽ��ճ���
	unsigned int dwRcvSize = 0;	//Ԥ�ƽ������ݳ���
	unsigned int dwLen = 0;		//�ܼƽ������ݳ���
	unsigned int dwDataRegionLen = 0; //�����򳤶�

	signed int ret = 0;
	unsigned char *pRxData = pBuf;
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
			if (pRxData[0] == 0x68)
			{
				dwRcvSize = 2;
				bStatus = FSM_STATUS_RX_FRMLEN;
				pRxData += dwRcvLen;
			}
			break;
			
		case FSM_STATUS_RX_FRMLEN:
			dwDataRegionLen = (pRxData[1]<<8) + pRxData[0]; 					//��ȥ�������ͷ3���ֽ�
			if (dwDataRegionLen > 0)
			{
				dwRcvSize = dwDataRegionLen;
				if (dwRcvSize > RX_DATA_MAX_LEN)
				{
					TRACE("dwDataRegionLen %d err len.\n\r", dwDataRegionLen);
					return false;
				}

				bStatus = FSM_STATUS_RX_FRMDATA;
				pRxData += dwRcvLen;
			}
			break;				

		case FSM_STATUS_RX_FRMDATA:
			ucCs = CheckSum(pBuf, dwLen-2);
			if ((pBuf[dwLen-1] == 0x16) && (pBuf[dwLen-2] == ucCs))
			{
				*pdwLen = dwLen;
				return true;
			}
			break;

		default:
			return false;
			break;

		}
		
		dwRcvLen = m_pIf->Recv(pRxData, dwRcvSize);
		if (dwRcvLen != dwRcvSize)
		{
			if (dwRcvLen < 0)
				m_fAdapterConnected = false;
			
			return false;
		}
		
		dwLen += dwRcvLen;
	}

	return false;
}


//*****************************************************************************
// ��������: Send
// ��������: Э�鷢������֡
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
bool CCctAppAdapter::Send(unsigned char *pBuf, unsigned int dwlen)
{
	if (m_pIf != NULL)
	{
		if (dwlen == m_pIf->Send(pBuf, dwlen))
		{
			return true;
		}

	}

	return false;

}

//*****************************************************************************
// ��������: RegisterAppAdapter
// ��������: ��APP�ַ��߳�ע��APPҵ��������
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
void RegisterAppAdapter(IOT_APP_ID_ENUM wAppId)
{
	//����APP��app_map
	;
}


//*****************************************************************************
// ��������: GetTerminalAddr
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
bool CCctAppAdapter::GetTerminalAddr(unsigned char *pAddr)
{
	if (pAddr == NULL)
	{
		//memset(pAddr, 0xff, TERMINAL_ADDR_SIZE);

		//memcpy(pAddr, g_pCctReader[CCTPLC_READER]->GetMainAddr(), 6);
		//memcpy(pAddr, m_bTmAddr, 6);
		return false;
	}

	if (//IsBufferAllVal(m_bTmAddr, TERMINAL_ADDR_SIZE, 0) ||
		IsBufferAllVal(m_bTmAddr, TERMINAL_ADDR_SIZE, 0xee) ||
		IsBufferAllVal(m_bTmAddr, TERMINAL_ADDR_SIZE, 0xff) ||
		IsBufferAllVal(m_bTmAddr, TERMINAL_ADDR_SIZE, 0x99))
	{
		TRACE("GetTerminalAddr err !.\n");
		return false;		
	}

	memcpy(pAddr, m_bTmAddr, TERMINAL_ADDR_SIZE);
	return true;
	
}

//*****************************************************************************
// ��������: AdapterDoSaveMainNode
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
void CCctAppAdapter::AdapterDoSaveMainNode(unsigned char *pBuf, unsigned int dwlen)
{
	if ((pBuf == NULL) || (dwlen == 0))
	{
		TRACE("AdapterDoSaveMainNode err !.\n");
		return;		
	}

	if (//IsBufferAllVal(pBuf, TERMINAL_ADDR_SIZE, 0) || 
		IsBufferAllVal(pBuf, TERMINAL_ADDR_SIZE, 0xee) || 
		IsBufferAllVal(pBuf, TERMINAL_ADDR_SIZE, 0xff) ||
		IsBufferAllVal(pBuf, TERMINAL_ADDR_SIZE, 0x99))
	{
		TRACE("AdapterDoSaveMainNode err !.\n");
		return;		
	}

	TRACEHEX("m_bTmAddr", pBuf, TERMINAL_ADDR_SIZE);
	memcpy(m_bTmAddr, pBuf, TERMINAL_ADDR_SIZE);
}

//*****************************************************************************
// ��������: CctAppSoftUpgrade
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
void CCctAppAdapter::CctAppSoftUpgrade()
{	
	TRACE("CctAppSoftUpgrade.\n");
	AppUpGrade();
}

void CCctAppAdapter::CctAppGetVersion()
{
	unsigned short wOutBufLen = 0;
	unsigned char pOutBuf[128] = {0};
	CCT_APP_INFO tAppVersion;

	memset(pOutBuf, 0, sizeof(pOutBuf));
	memset(&tAppVersion, 0, sizeof(CCT_APP_INFO));
	memcpy(&tAppVersion, &g_AppVersion, sizeof(CCT_APP_INFO));
	TRACE("CCctAppAdapter::CctAppGetVersion %s %d-%d-%d %d:%d:%d.\n", 
							tAppVersion.bAppName, 
							tAppVersion.tTime.wYear, 
							tAppVersion.tTime.bMonth,
							tAppVersion.tTime.bDay, 
							tAppVersion.tTime.bHour, 
							tAppVersion.tTime.bMinute, 
							tAppVersion.tTime.bSecond);

	if (true == MakeAdapterFrm(ADAPTER_CCT_APP_FN8, (unsigned char *)&tAppVersion, sizeof(CCT_APP_INFO), pOutBuf, &wOutBufLen))
	{
		//AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pOutBuf, wOutBufLen);
		AdapterSendPackage2Svr(pOutBuf, wOutBufLen);
	}
}

//*****************************************************************************
// ��������: AdapterSendPackage2Svr
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
signed int CCctAppAdapter::AdapterSendPackage2Svr(unsigned char *pBuf, unsigned short wLen)
{	
	if ((NULL == pBuf) || 
		(0 == wLen) || 
		!m_fAdapterConnected)
	{
		return (-1);
	}
	
	return AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pBuf, wLen);
}

