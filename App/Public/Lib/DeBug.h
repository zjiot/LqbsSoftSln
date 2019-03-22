//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : DeBug.cpp
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : 
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
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
