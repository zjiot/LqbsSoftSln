//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : AppAdapter.cpp
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
#include "AppAdapter.h"

CAppAdapter::CAppAdapter()
{

}


CAppAdapter::~CAppAdapter()
{

}


bool CAppAdapter::Init()
{
	m_fConnected = false;
	return true;
}


//*****************************************************************************
// 函数名称: Send
// 功能描述: 发送数据
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

bool CAppAdapter::Send(unsigned char *pBuf, unsigned int dwlen)
{
	return false;
}




