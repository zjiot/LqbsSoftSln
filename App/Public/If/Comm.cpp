//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : Comm.cpp
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : ���ڽӿ���
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************

#include "Comm.h"


CComm::CComm()
{
	
}

CComm::~CComm()
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
bool CComm::Init(TCommPara tCommPara)
{
	m_wPort = tCommPara.wPort;
	m_dwBaudRate = tCommPara.dwBaudRate;
	m_bByteSize = tCommPara.bByteSize;
	m_bStopBits = tCommPara.bStopBits;
	m_bParity = tCommPara.bParity;
	return Open(tCommPara.wPort, tCommPara.dwBaudRate, tCommPara.bByteSize, tCommPara.bStopBits, tCommPara.bParity);
}

//*****************************************************************************
// ��������: Open
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
bool CComm::Open(unsigned short wPort, unsigned int dwBaudRate, unsigned char bByteSize, unsigned char bStopBits, unsigned char bParity)
{
	
	if (0 != (Comm_fd = OpenComm(wPort, dwBaudRate, bByteSize, bStopBits, bParity)))
	{
		m_fOpen = true;
	}

	return false;
}

//*****************************************************************************
// ��������: Read
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
unsigned int CComm::Read(void* pBuf, unsigned short wLength)
{
	int error = 0;
	unsigned int ret = 0;
	unsigned char comm_ret = 0;
	
	if ((Comm_fd == 0) || (!IsOpen()))
	{
		return 0;
	}

	ret = CommReceive(Comm_fd, (unsigned char *)pBuf, wLength, &error);
	if(ret == 0 && error < 0)
	{
		//���ڳ����⣬try to reopen
		TRACE("read error try to reopen COM Comm_fd %d.\n", Comm_fd);
		Close();
		Sleep(500);
		comm_ret = Open(m_wPort,m_dwBaudRate,m_bByteSize,m_bStopBits,m_bParity);
		TRACE("reopen COM result[%d].\n", comm_ret);
	}

	return ret;
}

//*****************************************************************************
// ��������: Write
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
unsigned int CComm::Write(void* pBuf, unsigned short wLength)
{
	if ((Comm_fd == 0) || (!IsOpen()))
	{
		return 0;
	}

	if (true == CommSend(Comm_fd, (unsigned char *)pBuf, wLength))
	{
		return wLength;
	}

	return 0;
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
bool CComm::Close()
{
	bool ret = false;
	
	if ((Comm_fd == 0) || (!IsOpen()))
	{
		return 0;
	}

	ret = CloseComm(Comm_fd);
	if(ret)
	{
		m_fOpen = false;
	}

	return ret;
}