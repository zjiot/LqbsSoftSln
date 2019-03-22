//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : HisiReader.h
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 3762协议抄表
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
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

	//此接口是主动路由数据抄读接口 不必重写此接口
	virtual signed int  DoAutoCctReader(){return 0;};

	//被动路由抄读接口 海思是被动型路由AFN=13 FN=01 重写此接口
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


