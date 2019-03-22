//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : HisiReader.cpp
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : 3762Э�鳭��
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************
#include "HisiReader.h"
#include "CctCfg.h"

CHisiReader::CHisiReader()
{
	
}

CHisiReader::~CHisiReader()
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
bool CHisiReader::Init(TCctPara *pCctPara)
{
	return CCctReader::Init(pCctPara);
}

//*****************************************************************************
// ��������: CctReadItemData
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
signed int CHisiReader::CctReadItemData(unsigned char *pInBuf, unsigned int dwInBufLen, unsigned char *pOutBuf, unsigned int* pOutBufLen)
{
	//unsigned char a[] = {0x68, 0x2E, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x09, 0x17, 0x20, 0x02, 0x34, 0x01, 0x02, 0x02, 0xE8, 0x02, 
	//					0x00, 0x80, 0x46, 0x00, 0x10, 0x68, 0x02, 0x00, 0x00, 0x09, 0x17, 0x20, 0x68, 0x11, 0x04, 0x34, 0x34, 0x39, 0x38, 0x00, 0x16, 0xB3, 0x16};

	//unsigned char bFrm[] = {0x68, 0x02, 0x00, 0x00, 0x09, 0x17, 0x20, 0x68, 0x11, 0x04, 0x34, 0x34, 0x39, 0x38, 0x00, 0x16};

	int dwRet = CCT_ERR_SUCCESS;
	if ((pInBuf == NULL) || 
		(pOutBuf == NULL) || 
		(dwInBufLen == 0))
		return (-1);

	//dwInBufLen = sizeof(bFrm);
	//memcpy(pInBuf, bFrm, dwInBufLen);

#if defined(PRODUCT_CFG_PRODUCT_TYPE_MRS)
	if (CCT_ERR_SUCCESS == CCctReader::Make3762ReadFrm(m_pTx, &m_TxLen, CCT_AFN_3762_RTFWD, 1, pInBuf, dwInBufLen))
#endif		
#if defined(PRODUCT_CFG_PRODUCT_TYPE_CSG)
	if (CCT_ERR_SUCCESS == CCctReader::Make3762CsgReadFrm(m_pTx, &m_TxLen, 0x02, 0xe8020201, pInBuf, dwInBufLen))
#endif
	{
		//���Դ���
		//if (CCT_ERR_SUCCESS == CCctReader::Send(a, sizeof(a)))
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
signed int CHisiReader::Get645FrmSizeFrom3762Frm(unsigned char *p3762Frm, unsigned int *p645FrmSize)
{
	if ((NULL == p3762Frm) || (p645FrmSize == NULL))
		return (-1);

	//(68+ADDR+68+C+L  +CS+16 = 12)
	//*p645FrmSize = GetDataLen(p3762Frm + FRM_645_DATA_POS, CCT_MTRPROTO_07) + 12;
	*p645FrmSize = (unsigned char)*(p3762Frm + 28);
	return CCT_ERR_SUCCESS;
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
signed int CHisiReader::Send(unsigned char *pTx, unsigned int dwTxLen)
{
	return CCctReader::Send(pTx, dwTxLen);
}

//����:·��������������
#if 0
signed int CHisiReader::DoAutoCctReader()
{
	//if (!m_fInit)	//·��ģ�黹û��ʼ����ȷ
	//{
	//	ReInit();
	//	return 0;
	//}
	
	if (RxFrm(1))
	{
		ExRxFrm(); 
	}

	return 0;
}
#endif

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
void CHisiReader::DoCctReader(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize)
{
	//�����ѻָ�
	if (!m_dwDirReadClick)
	{
		//��׼��3762�ø�������Լ���
		CCctReader::DoCctReader(dwMsgId, pFrmData, dwFrmSize);
		
		//CCctReader::DoPointRead();
	}
}

