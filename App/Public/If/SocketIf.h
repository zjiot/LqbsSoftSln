//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : SocketIf.cpp
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : Socket接口类
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
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
	SOCKET m_Socket;					//若是服务器: 作为Accept的接口  若是客户端: 作为rcv\send的接口
	SOCKET m_cSocket;					//若是服务器: 作为Accept后返回的对远程客户端的rcv\send的接口   若是客户端: 此接口不使用
	unsigned char   m_bConnectNum;
	unsigned int m_dwTimeOut;
	bool  m_fLinkStatus;

	unsigned char  m_bIpAddr[4];
	unsigned int m_dwPort;

  
};

//*****************************************************************************
// 函数名称: LoadSvrSocketPara
// 功能描述: 服务器装载联网参数
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
void LoadSvrSocketPara(TSocketPara *pSocketPara);

#endif

