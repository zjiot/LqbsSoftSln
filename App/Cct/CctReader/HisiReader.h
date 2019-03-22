//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : HisiReader.h
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

#ifndef HISIREADER_H
#define HISIREADER_H
#pragma once

#include "CctReader.h"

#define FRM_645_DATA_POS			29



class CHisiReader : public CCctReader
{
public:
	CHisiReader();
	virtual ~CHisiReader();

	bool Init(TCctPara *pCctPara);

	//�˽ӿ�������·�����ݳ����ӿ� ������д�˽ӿ�
	virtual signed int  DoAutoCctReader(){return 0;};

	//����·�ɳ����ӿ� ��˼�Ǳ�����·��AFN=13 FN=01 ��д�˽ӿ�
	virtual void DoCctReader(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize);
	virtual signed int  CctReadItemData(unsigned char *pInBuf, unsigned int dwInBufLen, unsigned char *pOutBuf, unsigned int* pOutBufLen);
	virtual signed int Make3762ReadFrm(unsigned char* pbTxBuf, unsigned int *pdwLen, 
												unsigned char bAfn, unsigned short wFn, 
												unsigned char* pbData, unsigned short wDataLen){return 0;};
	virtual signed int Send(unsigned char *pTx, unsigned int dwTxLen);
	virtual bool RcvFrm(unsigned char *pRx, unsigned int *pRxLen){return 0;};

	signed int Get645FrmSizeFrom3762Frm(unsigned char *p3762Frm, unsigned int *p645FrmSize);

	
//private:


};


#endif


