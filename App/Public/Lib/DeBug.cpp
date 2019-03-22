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

#include "DeBug.h"
#include "SocketIf.h"


CSocketIf *pReMoteSocket = NULL;


//*****************************************************************************
// 函数名称: TRACEHEX
// 功能描述: 
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void TRACEHEX(const char *pszFormat, unsigned char *pBuf, unsigned short size)
{
	//if (!g_UartInit)return;
	if ((NULL == pBuf) || (0 == size))
		return;

	TTime tTime;
	GetTime(&tTime);

	if (pszFormat != NULL)
		TRACE("======================= %s =======================\n", pszFormat);

	char *pHex = NULL;
	if (size > 1000)  size = 1000;
	pHex = new char[size * 3 + 1];
	if (pHex != NULL)
	{
		for (int i = 0; i < size; i++)
		{
			sprintf(&pHex[3 * i], "%3x", pBuf[i]);
			if (pHex[3 * i + 1] == ' ') pHex[3 * i + 1] = '0';
		}
		pHex[size * 3] = 0;
		TRACE("[%d-%d-%d %d:%d:%d]%s\n", tTime.wYear, tTime.bMonth, 
			tTime.bDay, tTime.bHour, tTime.bMinute, tTime.bSecond, pHex);

		//同时送到远程调试端
		//REMOTETRACEHEX((const char *)pHex);

		delete[] pHex;
	}
	else
	{
		return;
	}


	if (pszFormat != NULL)
		TRACE("======================= %s =======================\n", pszFormat);

	TRACE("\n");
}

//*****************************************************************************
// 函数名称: REMOTETRACEHEX
// 功能描述: 
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void REMOTETRACEHEX(const char *pHex)
{
	if (pReMoteSocket->IsConnected())
	{
		pReMoteSocket->Send((unsigned char *)pHex, strlen(pHex));
	}
}

//*****************************************************************************
// 函数名称: REMOTETRACE
// 功能描述: 
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void REMOTETRACE(const char *s, ...)
{
	if (!pReMoteSocket->IsConnected())
	{
		return;
	}

	va_list ap;
	unsigned long arg;
	const char *str;
	char ch1, ch2, ch3;
	char lastch = 0;
	
	va_start(ap, s);
	//API_IO_WDI();
	while (*s != 0) 
	{
		if (*s == '%') 
		{
			s++;
			ch1 = *s++;
			if (ch1 == 's') 
			{
				str =(const char *) va_arg(ap, unsigned char *);
				puts_api(str);
			} 
			else if (ch1 == 'd') 
			{
				arg = va_arg(ap, unsigned long);
				print_d_api(arg);
			} 
			else if (ch1 == 'x') 
			{
				arg = va_arg(ap, unsigned long);
				print_x_api(arg);
			} 
			else if (ch1 == '\n') 
			{
				putch_api(0x0d);
			} 
			else if (ch1 == '\r') 
			{
				putch_api(0x0a);								
			} 
			else if(ch1 == '\0')
			{
				break;
			} 
			else 
			{
				ch2 = *s++;
				ch3 = *s++;
				arg = va_arg(ap, unsigned long);
				if(ch2 >= '0' && ch2 <= '9' && ch3 != '\0')
				{
					print_0nx_api(ch2, arg);
				}
				else
				{
					break;
				}
			}
		} 
		else
		{
			ch1 = *s++;
			if (ch1 == '\r') 
			{
				putch_api(0x0d);
			}
			else if (ch1 == '\n') 
			{
				if(lastch != '\r')
				{
					putch_api(0x0d);
				}
				putch_api(0x0a);								
			} 
			else if(ch1 == '\0')
			{
				break;
			}
			else
				putch_api(ch1);
			lastch = ch1;
		}
	}
	va_end(ap);

}

//*****************************************************************************
// 函数名称: puts_api
// 功能描述: 
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void puts_api(const char *s)
{
	while (*s != 0) 
	{
		putch_api(*s++);
	}

}

//*****************************************************************************
// 函数名称: print_d_api
// 功能描述: 
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void print_d_api(unsigned long l)
{
	unsigned long prod, t;
	int flag = 0;
	unsigned char ch;
	int i, j;

	for (i = 15; i >= 0; i--) 
	{
		prod = 1;
		t = l;
		for (j = 0; j < i; j++) 
		{
			prod = prod * 10;
			t = t / 10;
		}
		ch = t;
		l -= prod * t;

		if (ch == 0 && flag == 0)
			continue;

		if (ch < 10)
			putch_api(ch + '0');
		else
			putch_api('?');

		flag = 1;
	}
}

//*****************************************************************************
// 函数名称: print_x_api
// 功能描述: 
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void print_x_api(unsigned long l)
{
	int flag = 0;
	unsigned char ch;
	int i;

	for (i = 7; i >= 0; i--) 
	{
		
		ch = (l >> i)&0x0f;
		if(ch == 0 && flag == 0)
			continue;

		if (ch < 10)
			putch_api(ch + '0');
		else
			putch_api(ch - 10 + 'A');
		
		flag = 1;
	}
}

//*****************************************************************************
// 函数名称: print_0nx_api
// 功能描述: 
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void print_0nx_api(char ch2, unsigned long l)
{
	unsigned char ch;
	int i;

	ch2 = ch2 - '0';
	for (i = ch2 - 1; i >= 0; i--) 
	{
		ch = (l >> (i * 4)) & 0x0f;
		if (ch < 10)
			putch_api(ch + '0');
		else
			putch_api(ch - 10 + 'A');
	}
}

//*****************************************************************************
// 函数名称: putch_api
// 功能描述: 
//
// 参数说明:
// dwQueId  -  in，dwMsgId  -  in，pBuf  -  in，dwSize  -  in
// 
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void putch_api(int ch)
{
	if (pReMoteSocket->IsConnected())
	{
		pReMoteSocket->Send((unsigned char *)&ch, 1);
	}
}

//*****************************************************************************
// 函数名称: ReMoteTask
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
TTaskRet ReMoteTask(void *pvArg)
{
	#define SOCKET_REMOTE_STATUS_DISCONN	0
	#define SOCKET_REMOTE_STATUS_LISTEN		1
	#define SOCKET_REMOTE_STATUS_ACCEPT		2
	#define SOCKET_REMOTE_STATUS_CONNECT 	3
	
	unsigned char bStatus = SOCKET_REMOTE_STATUS_DISCONN;

	TSocketPara tSocketPara;
	memset(&tSocketPara, 0, sizeof(TSocketPara));
	
	LoadSvrSocketPara(&tSocketPara);
	pReMoteSocket = new CSocketIf();
	if (pReMoteSocket == NULL)
	{
		TRACE("ReMoteTask new CSocketIf err.\n");
	}

	for (;;)
	{
		switch(bStatus)
		{
		case SOCKET_REMOTE_STATUS_DISCONN:
			if (pReMoteSocket != NULL)
			{
				pReMoteSocket->Init(tSocketPara.bIP, tSocketPara.dwPort);
			}
			
			bStatus = SOCKET_REMOTE_STATUS_LISTEN;
			break;

		case SOCKET_REMOTE_STATUS_LISTEN:
			if (pReMoteSocket->WaitConnect())
			{
				if (pReMoteSocket->IsConnected()) 
				{
					bStatus = SOCKET_REMOTE_STATUS_CONNECT;
				}
			}
			break;	

		case SOCKET_REMOTE_STATUS_CONNECT:
			if (!pReMoteSocket->IsConnected()) 
			{
				bStatus = SOCKET_REMOTE_STATUS_DISCONN;	
				pReMoteSocket->Close();
			}
			break;	

		default:
			TRACE("ReMoteTask bStatus err !.\n");
			break;			
		}

		Sleep(100);
	}

}


