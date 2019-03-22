//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : AppAdapter.h
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : ҵ��ģ������������
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
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

    //void AttachIf(CIf* pIf) { m_pIf = pIf; }; //�󶨽ӿ�

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
	//CIf m_pIf;		//���еĽӿ�
	bool m_fConnected;

};


#endif


