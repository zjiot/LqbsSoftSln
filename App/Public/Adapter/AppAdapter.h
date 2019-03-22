//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : AppAdapter.h
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 业务模型适配器基类
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************
#ifndef PROTO_H
#define PROTO_H

#include "Comm.h"
#include "SocketIf.h"



class CAppAdapter
{
public:
	CAppAdapter();
	~CAppAdapter();
		
	bool Init();

    //void AttachIf(CIf* pIf) { m_pIf = pIf; }; //绑定接口

	virtual bool Login() { return true; };
	virtual bool Logoff() { return true; };
	virtual bool HeartBeat() { return true; };

	virtual bool RcvFrm(unsigned char *pBuf, unsigned int dwlen) { return 0; };
	virtual bool Send(unsigned char *pBuf, unsigned int dwlen);
	
	virtual bool IsAdapterConnected() { return true; };
	virtual bool IsNededToConnected() { return false; };

	//virtual void RegisterAppAdapter(IOT_APP_ID_ENUM wAppId);
	
	virtual signed int DispatchPackage(unsigned char *pBuf, unsigned short wLen) { return 0; };
	virtual bool MakeFrm(unsigned short wCmdCode, unsigned char *pBuf, unsigned short wLen) { return 0; };
	
private:
	//CIf m_pIf;		//持有的接口
	bool m_fConnected;

};


#endif


