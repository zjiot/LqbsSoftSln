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

#include "SocketIf.h"

CSocketIf::CSocketIf()
{

}

CSocketIf::~CSocketIf()
{
	if (m_cSocket)
	{
		SocketClose(&m_cSocket);
	}

	SocketClose(&m_Socket);
	m_bConnectNum = 0;
}

//*****************************************************************************
// ��������: ServerInit
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
void CSocketIf::Init(unsigned char *pIpAddr, unsigned int dwPort)
{
	m_fLinkStatus = false;
	memcpy(m_bIpAddr, pIpAddr, sizeof(m_bIpAddr));
	m_dwPort = dwPort;
	
	m_cSocket = ERR_SOCKET_INVALID;
	m_Socket = ERR_SOCKET_INVALID;
	
	if(pIpAddr[0] > 0)
	{
		ClientInit(pIpAddr, dwPort);
	}
	else
	{
		m_bConnectNum = 0;
		ServerInit(dwPort);
	}
}

//*****************************************************************************
// ��������: ServerInit
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
void CSocketIf::ServerInit(unsigned int dwPort)
{
    signed int dwRet = SOCKET_ERR_SUCCESS;
    sockaddr_in sockAddr = {0};

    m_cSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(ERR_SOCKET_INVALID != m_cSocket)
    {
        signed int dwReUse = 1;
        if(SOCKET_ERR_SUCCESS != setsockopt(m_cSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&dwReUse, sizeof(signed int)))
        {
            TRACE("setsockopt");
        }

        sockAddr.sin_port = htons(dwPort);
        if(0 != sockAddr.sin_port)
        {
            sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            sockAddr.sin_family = AF_INET;

            dwRet = bind(m_cSocket, (const sockaddr*) &sockAddr, sizeof(sockaddr_in));
            if(SOCKET_ERR_FAILED == dwRet)
            {
                TRACE("failed while bind\n");
            }
            else
            {
                dwRet = listen(m_cSocket, 0); 
                if(SOCKET_ERR_FAILED == dwRet)
                {
                    SocketClose(&m_cSocket);
                }
            }
            if(dwRet == SOCKET_ERR_FAILED)
            {
                SocketClose(&m_cSocket);
            }
        }
    }
    else
    {
        TRACE("failed while socket()\n");
    }
}

//*****************************************************************************
// ��������: WaitConnect
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
SOCKET CSocketIf::WaitConnect()
{
    sockaddr rec_addr;
    socklen_t rec_len = sizeof(sockaddr);
    m_Socket = accept(m_cSocket, &rec_addr, &rec_len);
    if(m_bConnectNum)
    {
        SocketClose(&m_Socket);
    }
    else
    {
        m_bConnectNum = 1;		
		m_fLinkStatus = true;
    }
    return m_Socket;
}

//*****************************************************************************
// ��������: ClientInit
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
void CSocketIf::ClientInit(unsigned char *pIpAddr, unsigned int dwPort)
{
    char bAddr[32] = {0};

    signed int dwRet;

    if(m_Socket != ERR_SOCKET_INVALID)
    {
        SocketClose(&m_Socket);
    }
    m_Socket = socket(AF_INET, SOCK_STREAM, 0);
    for(signed int i = 0; i < 4; i++)
    {
        //sprintf((char *)&bAddr[strlen((char *)bAddr)], "%d", (const char * const)&pIpAddr[i]);
        //strcat((char *)bAddr, ".");

		sprintf(&bAddr[strlen((const char *)bAddr)], "%d", pIpAddr[i]);
		strcat((char *)bAddr, ".");

    }
    bAddr[strlen((char *)bAddr) - 1] = 0;

	TRACE("CSocketIf::ClientInit IP %s:%d\n", bAddr, dwPort);

    if(m_Socket != ERR_SOCKET_INVALID)
    {
        struct sockaddr_in sAddr;

        sAddr.sin_family = AF_INET;
        sAddr.sin_port = htons(dwPort);
        
		
		sAddr.sin_addr.s_addr = inet_addr((const char *)bAddr);
		//sAddr.sin_addr.s_addr = ConvertAddr2IP((const char *)bAddr);

		//SetSocketNoBlock(m_Socket);

        dwRet = connect(m_Socket, (sockaddr*)(&sAddr), sizeof(sockaddr));
		TRACE("CSocketIf::ClientInit connect dwRet %d.\n",dwRet);
        if(ERR_SOCKET_INVALID == dwRet)
        {
            SocketClose(&m_Socket);
        }
		else
		{
			m_fLinkStatus = true;
		}
    }
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
signed int CSocketIf::Send(unsigned char *pBuf, unsigned int dwLen)
{
    SOCKET socket;
    signed int dwRet = SOCKET_ERR_SUCCESS;
	unsigned int dwTimeout = m_dwTimeOut;

    socket = m_Socket;

    fd_set fs_write;

    FD_ZERO(&fs_write);
    FD_SET(socket, &fs_write);

	struct timeval tv_timeout;
    //tv_timeout.tv_sec = TIMEOUT_USEC;
	tv_timeout.tv_sec = 0;
	tv_timeout.tv_usec = dwTimeout * 1000;

    if((dwRet = select(socket + 1, NULL, &fs_write, NULL, &tv_timeout)) > 0)
    {
        if(FD_ISSET(socket, &fs_write))
        {
            dwRet = (unsigned int)send(socket, (const char *)pBuf, dwLen, 0);
			if (dwRet != dwLen)
			{
				m_fLinkStatus = false;
			}
        }
    }
    else
    {
        dwRet = SOCKET_ERR_FAILED;		
		m_fLinkStatus = false;
    }

    return dwRet;
}

//*****************************************************************************
// ��������: Recv
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
signed int CSocketIf::Recv(unsigned char *pBuf, unsigned int dwLen)
{
    signed int dwRet = SOCKET_ERR_SUCCESS;
    SOCKET socket;
	unsigned int dwTimeout = m_dwTimeOut;

    socket = m_Socket;
    if(socket == 0)
    {
        socket = m_Socket;
    }

    fd_set fs_Read; 
    struct timeval tv_timeout;

    FD_ZERO(&fs_Read);
    FD_SET(socket, &fs_Read);

    //tv_timeout.tv_sec = TIMEOUT_USEC;
	tv_timeout.tv_sec = 0;
	
	tv_timeout.tv_usec = dwTimeout * 1000;

    unsigned int nTot = 0;
		
    do
    {
        if((dwRet = select(socket + 1, &fs_Read, NULL, NULL, &tv_timeout)) > 0)
        {
            if(FD_ISSET(socket, &fs_Read))
            {
				//dwRet = recv(socket, &pBuf[nTot++], 1, MSG_WAITALL);
                dwRet = recv(socket, (char *)&pBuf[nTot++], 1, MSG_WAITALL);
                if(dwRet <= 0)
                {
					m_fLinkStatus = false;
                    dwRet =  - 1;
                }
            }
        }
    }while(dwRet > 0 && nTot < dwLen);
		
    if(dwRet > 0)
    {
        dwRet = nTot;
    }

    return dwRet;
}

//*****************************************************************************
// ��������: SocketClose
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
void CSocketIf::SocketClose(SOCKET *s)
{
    if(*s != ERR_SOCKET_INVALID)
    {
		signed int dwRet = SOCKET_ERR_SUCCESS;
		//dwRet = shutdown(*s, SHUT_RDWR);
		dwRet = SocketShutDown(s);
    	if(dwRet != SOCKET_ERR_SUCCESS) TRACE("socket shutdown.\n");
		dwRet = CloseSocket((int *)s);
   		if(dwRet != SOCKET_ERR_SUCCESS) TRACE("socket close.\n");
        if(*s != ERR_SOCKET_INVALID)
        {
            *s = ERR_SOCKET_INVALID;
        }
    }
}

//*****************************************************************************
// ��������: Close
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
void CSocketIf::Close()
{
    if(m_Socket != ERR_SOCKET_INVALID)
    {
		signed int dwRet = SOCKET_ERR_SUCCESS;
		//dwRet = shutdown(*s, SHUT_RDWR);
		dwRet = SocketShutDown(&m_Socket);
    	if(dwRet != SOCKET_ERR_SUCCESS) TRACE("socket shutdown");
		dwRet = CloseSocket((int *)&m_Socket);
   		if(dwRet != SOCKET_ERR_SUCCESS) TRACE("socket close");
        if(m_Socket != ERR_SOCKET_INVALID)
        {
            m_Socket = ERR_SOCKET_INVALID;
        }
    }
	
    if(m_cSocket != ERR_SOCKET_INVALID)
    {
		signed int dwRet = SOCKET_ERR_SUCCESS;
		//dwRet = shutdown(*s, SHUT_RDWR);
		dwRet = SocketShutDown(&m_cSocket);
    	if(dwRet != SOCKET_ERR_SUCCESS) TRACE("socket shutdown");
		dwRet = CloseSocket((int *)&m_cSocket);
   		if(dwRet != SOCKET_ERR_SUCCESS) TRACE("socket close");
        if(m_cSocket != ERR_SOCKET_INVALID)
        {
            m_cSocket = ERR_SOCKET_INVALID;
        }
    }
	
}


//*****************************************************************************
// ��������: SocketReConnect
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
void CSocketIf::SocketReConnect()
{
	return ClientInit(m_bIpAddr, m_dwPort);
}


//*****************************************************************************
// ��������: IsConnected
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
bool CSocketIf::IsConnected()
{
	return m_fLinkStatus;
}

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
void LoadSvrSocketPara(TSocketPara *pSocketPara)
{
	unsigned char bIP[4] = { 0,0,0,0 };
	memcpy(pSocketPara->bIP, bIP, sizeof(bIP));
	pSocketPara->dwPort = 55951;
}