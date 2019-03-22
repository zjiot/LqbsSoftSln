//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : SocketIf.cpp
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : Socket�ӿ���
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************

#ifndef SOCKETIF_H
#define SOCKETIF_H

//#include "Cct.h"
#include "arch.h"


#define ERR_SOCKET_INVALID			(-1)

typedef struct
{
	unsigned char bIP[4];
	unsigned int dwPort;


}TSocketPara;
 
class CSocketIf
{
public:
	CSocketIf();
	~CSocketIf();

	void   Init(unsigned char *pIpAddr, unsigned int dwPort);
	void   ServerInit(unsigned int dwPort);
	void   ClientInit(unsigned char* pAddr, unsigned int dwPort);
	bool   IsConnected();
	SOCKET WaitConnect();
	void   SocketClose(SOCKET *s);
	void   SocketReConnect();
	void   Close();

	signed int	Recv(unsigned char *pBuf,unsigned int dwLen);  
	signed int Send(unsigned char *pBuf,unsigned int dwLen);

private:
	SOCKET m_Socket;					//���Ƿ�����: ��ΪAccept�Ľӿ�  ���ǿͻ���: ��Ϊrcv\send�Ľӿ�
	SOCKET m_cSocket;					//���Ƿ�����: ��ΪAccept�󷵻صĶ�Զ�̿ͻ��˵�rcv\send�Ľӿ�   ���ǿͻ���: �˽ӿڲ�ʹ��
	unsigned char   m_bConnectNum;
	unsigned int m_dwTimeOut;
	bool  m_fLinkStatus;

	unsigned char  m_bIpAddr[4];
	unsigned int m_dwPort;

  
};

//*****************************************************************************
// ��������: LoadSvrSocketPara
// ��������: ������װ����������
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
void LoadSvrSocketPara(TSocketPara *pSocketPara);

#endif

