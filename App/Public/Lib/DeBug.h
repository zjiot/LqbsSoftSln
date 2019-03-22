//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : DeBug.cpp
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "stdio.h"
#include <stdarg.h>

//#include "CctMain.h"
#include "SocketIf.h"

extern CSocketIf *pReMoteSocket;


void TRACEHEX(const char *pszFormat, unsigned char *pBuf, unsigned short size);
void REMOTETRACEHEX(const char *pHex);
void REMOTETRACE(const char *s, ...);

void puts_api(const char *s);
void print_d_api(unsigned long l);
void print_x_api(unsigned long l);
void print_0nx_api(char ch2, unsigned long l);
void putch_api(int ch);

TTaskRet ReMoteTask(void *pvArg);

#endif
