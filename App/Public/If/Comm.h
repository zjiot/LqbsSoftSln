//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : Comm.h
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
#ifndef CCOMMIF_H
#define CCOMMIF_H
//#include "Cct.h"
#include "arch.h"



class CComm
{
public:
	CComm();
	virtual ~CComm();


	bool Init(TCommPara tCommPara);
	void Config(TCommPara &CommPara){return;};
	bool Open(unsigned short wPort, unsigned int dwBaudRate, unsigned char bByteSize,	unsigned char bStopBits, unsigned char bParity);
  	
  	bool Close();
  	bool SetComm(unsigned int dwBaudRate, unsigned char bByteSize, unsigned char bStopBits, unsigned char bParity){return true;};
	bool SetBaudRate(unsigned int dwBaudRate){return true;};
	signed int GetCommFd(){return Comm_fd;};
	
	unsigned int Read(void* pvBuf, unsigned short wLength);
	unsigned int Write(void* pvBuf, unsigned short wLength);

	
  	bool  IsOpen() { return m_fOpen; };	
	bool SetDTR(){  return true;};
	bool ClrDTR(){  return true;};
	bool GetRI(void) { return false;};
	void SetTimeouts(unsigned int dwTimeouts) { m_dwTimeouts = dwTimeouts;};
	
	void CreateSemaphore(void){return;};
	void DeleteSemaphore(void){return;};

private:
	unsigned short  	m_wPort;
	unsigned int 	m_dwTimeouts;
	unsigned int 	m_dwBaudRate; 
	unsigned char  	m_bByteSize;
	unsigned char  	m_bStopBits;
	unsigned char  	m_bParity;
	bool  	m_fOpen;
	signed int    Comm_fd;	
	
};


#endif
