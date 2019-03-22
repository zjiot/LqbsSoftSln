//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : Comm.cpp
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 串口接口类
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
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
// 函数名称: Init
// 功能描述: 
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
// 函数名称: Open
// 功能描述: 
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
bool CComm::Open(unsigned short wPort, unsigned int dwBaudRate, unsigned char bByteSize, unsigned char bStopBits, unsigned char bParity)
{
	
	if (0 != (Comm_fd = OpenComm(wPort, dwBaudRate, bByteSize, bStopBits, bParity)))
	{
		m_fOpen = true;
	}

	return false;
}

//*****************************************************************************
// 函数名称: Read
// 功能描述: 
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
		//串口出问题，try to reopen
		TRACE("read error try to reopen COM Comm_fd %d.\n", Comm_fd);
		Close();
		Sleep(500);
		comm_ret = Open(m_wPort,m_dwBaudRate,m_bByteSize,m_bStopBits,m_bParity);
		TRACE("reopen COM result[%d].\n", comm_ret);
	}

	return ret;
}

//*****************************************************************************
// 函数名称: Write
// 功能描述: 
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
// 函数名称: Close
// 功能描述: 
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