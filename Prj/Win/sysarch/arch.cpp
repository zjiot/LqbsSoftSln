//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : arch.cpp
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 操作系统接口差异
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************
#include "arch.h"
#include "Mq.h"
#include "MsgQueue.h"

#include <Mmsystem.h>
#pragma comment( lib,"winmm.lib" )

#if 0
const char *GetQueueName(signed int dwQueueId)
{
	return 0;
}
#endif

 //*****************************************************************************
 // 函数名称: NewTask
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
 // 作	  者: sUesTcxF
 //*****************************************************************************
void NewTask(TTaskRet(*pFunc)(void *arg), void *pvArg, int nStackSize, TPrio prio)
{
	AfxBeginThread((AFX_THREADPROC)pFunc, pvArg, prio, nStackSize);
}

//*****************************************************************************
// 函数名称: NewTaskEX
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
// 作	  者: sUesTcxF
//*****************************************************************************
unsigned int NewTaskEX(TTaskRet(*pFunc)(void *arg), void *pvArg, int nStackSize, TPrio prio)
{
	unsigned int dwThrID = 0;
	_beginthreadex(NULL, 0, (_beginthreadex_proc_type)pFunc, NULL, 0, &dwThrID);
	return dwThrID;
}

//*****************************************************************************
// 函数名称: CharTowChar
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
// 作	  者: sUesTcxF
//*****************************************************************************
void CharTowChar(const char *chr, wchar_t *wchar, int size)
{
	MultiByteToWideChar(CP_ACP, 0, chr, strlen(chr) + 1, wchar, size / sizeof(wchar[0]));
}

//*****************************************************************************
// 函数名称: NewQueue
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
// 作	  者: sUesTcxF
//*****************************************************************************
//成功返回消息队列句柄，失败返回0
signed int NewQueue(unsigned short wQueue, char *pszName, short dwMsgArrayCnt)//队列名称,队列最大消息条数
{
	if (NULL == pszName)return 0;

	HRESULT hRet = MQ_OK;
	QUEUEHANDLE hQueue;

	DWORD dwOutNameLength = 256;
	WCHAR wszOutName[256];

	MQPROPVARIANT  aQueuePropVar[2];
	QUEUEPROPID    aQueuePropId[2];
	HRESULT        aQueueStatus[2];

	//初始化队列属性
	DWORD cPropId = 0;
	aQueuePropId[cPropId] = PROPID_Q_PATHNAME;
	aQueuePropVar[cPropId].vt = VT_LPWSTR;

	//路径名和队列名拼接
	char *pszPathName = new char[256];
	if (NULL == pszPathName)return 0;
	memset(pszPathName, 0, 256);
	memcpy(pszPathName, ".\\PRIVATE$\\", strlen(".\\PRIVATE$\\"));
	memcpy(pszPathName + strlen(pszPathName), pszName, strlen(pszName));

	//转宽字符
	LPWSTR pszQueue = new WCHAR[256];
	if (NULL == pszQueue)return 0;
	memset(pszQueue, 0, sizeof(WCHAR) * 256);
	LPWSTR pszQueueBak = new WCHAR[256];
	if (NULL == pszQueueBak)return 0;
	memset(pszQueueBak, 0, sizeof(WCHAR) * 256);
	CharTowChar((const char *)pszPathName, pszQueue, sizeof(wchar_t)*strlen(pszPathName));
	aQueuePropVar[cPropId].pwszVal = pszQueue;

	cPropId++;

	WCHAR wszLabel[MQ_MAX_Q_LABEL_LEN] = L"The_message_label";
	aQueuePropId[cPropId] = PROPID_Q_LABEL;
	aQueuePropVar[cPropId].vt = VT_LPWSTR;
	aQueuePropVar[cPropId].pwszVal = wszLabel;
	cPropId++;

	MQQUEUEPROPS   QueueProps;
	QueueProps.cProp = cPropId;               // Number of properties  
	QueueProps.aPropID = aQueuePropId;        // IDs of the queue properties  
	QueueProps.aPropVar = aQueuePropVar;      // Values of the queue properties  
	QueueProps.aStatus = aQueueStatus;        // Pointer to the return status  

	//创建的队列已经存在 获取队列路径先删除
	//获取计算机名称
	TCHAR szComputerName[256];
	char ComputerName[256];
	memset(ComputerName, 0, sizeof(ComputerName));
	int size = 256;
	GetComputerName(szComputerName, (LPDWORD)&size);
	int dwNum = WideCharToMultiByte(CP_OEMCP, NULL, szComputerName, -1, NULL, 0, NULL, FALSE);
	WideCharToMultiByte(CP_OEMCP, NULL, szComputerName, -1, ComputerName, dwNum, NULL, FALSE);

	memset(pszPathName, 0, 256);
	memcpy(pszPathName, "DIRECT=OS:", strlen("DIRECT=OS:"));
	memcpy(pszPathName + strlen(pszPathName), ComputerName, strlen(ComputerName));
	memcpy(pszPathName + strlen(pszPathName), "\\PRIVATE$\\", strlen("\\PRIVATE$\\"));

	memcpy(pszPathName + strlen(pszPathName), pszName, strlen(pszName));
	memset(pszQueueBak, 0, sizeof(WCHAR) * 256);
	CharTowChar((const char *)pszPathName, pszQueueBak, sizeof(wchar_t)*strlen(pszPathName));
	hRet = MQDeleteQueue(pszQueueBak);

	hRet = MQCreateQueue(NULL, &QueueProps, wszOutName, &dwOutNameLength);
	if (hRet != MQ_OK)return 0;

	hRet = MQOpenQueue(wszOutName, MQ_SEND_ACCESS, MQ_DENY_NONE, &hQueue);
	if (hRet != MQ_OK)return 0;

	if (NULL != pszPathName)
	{
		delete pszPathName;
		pszPathName = NULL;
	}

	if (NULL != pszQueue)
	{
		delete pszQueue;
		pszQueue = NULL;
	}

	if (NULL != pszQueueBak)
	{
		delete pszQueueBak;
		pszQueueBak = NULL;
	}

	if (hRet == MQ_OK)return (int)hQueue;
	return 0;
}

//*****************************************************************************
// 函数名称: OpenQueue
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
// 作	  者: sUesTcxF
//*****************************************************************************
signed int OpenQueue(char *pszName, unsigned int dwAccess, short dwMsgArrayCnt)
{
	if (NULL == pszName)return 0;

	HRESULT hRet = MQ_OK;
	QUEUEHANDLE hQueue;

	DWORD dwOutNameLength = 256;
	WCHAR wszOutName[256];

	MQPROPVARIANT  aQueuePropVar[2];
	QUEUEPROPID    aQueuePropId[2];
	HRESULT        aQueueStatus[2];

	//初始化队列属性
	DWORD cPropId = 0;
	aQueuePropId[cPropId] = PROPID_Q_PATHNAME;
	aQueuePropVar[cPropId].vt = VT_LPWSTR;

	//路径名和队列名拼接
	char *pszPathName = new char[256];
	if (NULL == pszPathName)return 0;
	memset(pszPathName, 0, 256);
	memcpy(pszPathName, ".\\PRIVATE$\\", strlen(".\\PRIVATE$\\"));
	memcpy(pszPathName + strlen(pszPathName), pszName, strlen(pszName));

	//转宽字符
	LPWSTR pszQueue = new WCHAR[256];
	if (NULL == pszQueue)return 0;
	memset(pszQueue, 0, sizeof(WCHAR) * 256);
	CharTowChar((const char *)pszPathName, pszQueue, sizeof(wchar_t)*strlen(pszPathName));
	aQueuePropVar[cPropId].pwszVal = pszQueue;

	cPropId++;

	WCHAR wszLabel[MQ_MAX_Q_LABEL_LEN] = L"The_message_label";
	aQueuePropId[cPropId] = PROPID_Q_LABEL;
	aQueuePropVar[cPropId].vt = VT_LPWSTR;
	aQueuePropVar[cPropId].pwszVal = wszLabel;
	cPropId++;

	MQQUEUEPROPS   QueueProps;
	QueueProps.cProp = cPropId;               // Number of properties  
	QueueProps.aPropID = aQueuePropId;        // IDs of the queue properties  
	QueueProps.aPropVar = aQueuePropVar;      // Values of the queue properties  
	QueueProps.aStatus = aQueueStatus;        // Pointer to the return status  

											  //创建消息队列
	hRet = MQCreateQueue(NULL, &QueueProps, wszOutName, &dwOutNameLength);
	if ((hRet != MQ_OK) && (hRet != MQ_ERROR_QUEUE_EXISTS))return 0;

	//创建的队列已经存在 获取队列路径
	//获取计算机名称
	TCHAR szComputerName[256];
	char ComputerName[256];
	memset(ComputerName, 0, sizeof(ComputerName));
	int size = 256;
	GetComputerName(szComputerName, (LPDWORD)&size);

	int dwNum = WideCharToMultiByte(CP_OEMCP, NULL, szComputerName, -1, NULL, 0, NULL, FALSE);
	WideCharToMultiByte(CP_OEMCP, NULL, szComputerName, -1, ComputerName, dwNum, NULL, FALSE);

	memset(pszPathName, 0, 256);
	memcpy(pszPathName, "DIRECT=OS:", strlen("DIRECT=OS:"));
	memcpy(pszPathName + strlen(pszPathName), ComputerName, strlen(ComputerName));
	memcpy(pszPathName + strlen(pszPathName), "\\PRIVATE$\\", strlen("\\PRIVATE$\\"));

	memcpy(pszPathName + strlen(pszPathName), pszName, strlen(pszName));
	memset(pszQueue, 0, sizeof(WCHAR) * 256);
	CharTowChar((const char *)pszPathName, pszQueue, sizeof(wchar_t)*strlen(pszPathName));

	//打开消息队列 返回消息队列句柄hQueue作为队列ID
	if (MQ_ERROR_QUEUE_EXISTS == hRet)
		hRet = MQOpenQueue(pszQueue, dwAccess, MQ_DENY_NONE, &hQueue);
	else
		hRet = MQOpenQueue(wszOutName, dwAccess, MQ_DENY_NONE, &hQueue);

	if (NULL != pszPathName)
	{
		delete pszPathName;
		pszPathName = NULL;
	}

	if (NULL != pszQueue)
	{
		delete pszQueue;
		pszQueue = NULL;
	}

	if (hRet == MQ_OK)return (int)hQueue;
	return 0;
}

//*****************************************************************************
// 函数名称: SendMessage2Queue
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
// 作	  者: sUesTcxF
//*****************************************************************************
//成功返回0 失败-1
signed int SendMessage2Queue(int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, int dwTimeOut)
{

	if (NULL == pstMsg)return(-1);

	// Define the required constants and variables.  
	const int NUMBEROFPROPERTIES = 5;                     // Number of properties  
	DWORD cPropId = 0;                                    // Property counter  
	HRESULT hRet = MQ_OK;                                 // Return code  
	HANDLE hQueue = NULL;								  // Queue handle  

														  // Define an MQMSGPROPS structure.
	MQMSGPROPS msgProps;
	MSGPROPID aMsgPropId[NUMBEROFPROPERTIES];
	MQPROPVARIANT aMsgPropVar[NUMBEROFPROPERTIES];
	HRESULT aMsgStatus[NUMBEROFPROPERTIES];

	// Specify the message properties to be sent.
	aMsgPropId[cPropId] = PROPID_M_LABEL;                // Property ID
	aMsgPropVar[cPropId].vt = VT_LPWSTR;				 // Type indicator
	aMsgPropVar[cPropId].pwszVal = L" The_message_label";// The message label
	cPropId++;

	aMsgPropId[cPropId] = PROPID_M_BODY;                 // Property ID
	aMsgPropVar[cPropId].vt = VT_VECTOR | VT_UI1;        // Type indicator
	aMsgPropVar[cPropId].caub.pElems = (LPBYTE)pstMsg;
	aMsgPropVar[cPropId].caub.cElems = sizeof(CCT_QUEUE_MSG_S);
	cPropId++;

	DWORD dwBodyType = VT_BSTR;
	aMsgPropId[cPropId] = PROPID_M_BODY_TYPE;            // Property ID
	aMsgPropVar[cPropId].vt = VT_UI4;                    // Type indicator
	aMsgPropVar[cPropId].ulVal = dwBodyType;
	cPropId++;

	msgProps.cProp = cPropId;
	msgProps.aPropID = aMsgPropId;
	msgProps.aPropVar = aMsgPropVar;
	msgProps.aStatus = aMsgStatus;

	hQueue = (HANDLE)OpenQueue((char *)GetQueueName(dwQueueId), MQ_SEND_ACCESS, 0);

	if (NULL != hQueue)
	{
		hRet = MQSendMessage(hQueue, &msgProps, MQ_NO_TRANSACTION);

		MQCloseQueue(hQueue);
	}

	if (MQ_OK == hRet)return 0;
	return (-1);

}

//*****************************************************************************
// 函数名称: WaitMessage
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
// 作	  者: sUesTcxF
//*****************************************************************************
//成功返回0 失败-1
signed int WaitMessage(int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, unsigned int dwTimeOut)
{
	DWORD dwMsgTimeOut = INFINITE;
	if (NULL == pstMsg)return(-1);

	// Define the required constants and variables.  
	const int NUMBEROFPROPERTIES = 5;
	DWORD cPropId = 0;
	HRESULT hRet = (-1);                               // Return code  
	HANDLE hQueue = (HANDLE)dwQueueId;                  // Queue handle  
	ULONG ulBufferSize = sizeof(CCT_QUEUE_MSG_S);

	// Define an MQMSGPROPS structure.  
	MQMSGPROPS msgprops;
	MSGPROPID aMsgPropId[NUMBEROFPROPERTIES];
	MQPROPVARIANT aMsgPropVar[NUMBEROFPROPERTIES];
	HRESULT aMsgStatus[NUMBEROFPROPERTIES];

	// Specify the message properties to be retrieved.  
	aMsgPropId[cPropId] = PROPID_M_LABEL_LEN;           // Property ID  
	aMsgPropVar[cPropId].vt = VT_UI4;                   // Type indicator  
	aMsgPropVar[cPropId].ulVal = MQ_MAX_MSG_LABEL_LEN;  // Length of label  
	cPropId++;

	WCHAR wszLabelBuffer[MQ_MAX_MSG_LABEL_LEN];         // Label buffer  
	aMsgPropId[cPropId] = PROPID_M_LABEL;               // Property ID  
	aMsgPropVar[cPropId].vt = VT_LPWSTR;                // Type indicator  
	aMsgPropVar[cPropId].pwszVal = wszLabelBuffer;      // Label buffer  
	cPropId++;

	memset(pstMsg, 0, ulBufferSize);
	aMsgPropId[cPropId] = PROPID_M_BODY_SIZE;           // Property ID  
	aMsgPropVar[cPropId].vt = VT_NULL;                  // Type indicator  
	cPropId++;

	aMsgPropId[cPropId] = PROPID_M_BODY;                // Property ID  
	aMsgPropVar[cPropId].vt = VT_VECTOR | VT_UI1;       // Type indicator  
	aMsgPropVar[cPropId].caub.pElems = (UCHAR *)pstMsg; // Body buffer  
	aMsgPropVar[cPropId].caub.cElems = ulBufferSize;    // Buffer size  
	cPropId++;

	aMsgPropId[cPropId] = PROPID_M_BODY_TYPE;           // Property ID  
	aMsgPropVar[cPropId].vt = VT_NULL;                  // Type indicator  
	cPropId++;

	// Initialize the MQMSGPROPS structure.  
	msgprops.cProp = cPropId;                           // Number of message properties  
	msgprops.aPropID = aMsgPropId;                      // IDs of the message properties  
	msgprops.aPropVar = aMsgPropVar;                    // Values of the message properties  
	msgprops.aStatus = aMsgStatus;                      // Error reports  

	aMsgPropVar[0].ulVal = MQ_MAX_MSG_LABEL_LEN;

	if (0xffffffff == dwTimeOut)
		dwMsgTimeOut = INFINITE;//阻塞
	else
		dwMsgTimeOut = 0;		//非阻塞
		
	hQueue = (HANDLE)OpenQueue((char *)GetQueueName(dwQueueId), MQ_RECEIVE_ACCESS, 0);
	if (NULL != hQueue)
	{
		hRet = MQReceiveMessage(hQueue,					// Queue handle  
			dwMsgTimeOut,                  // Max time to (msec) to receive the message  
			MQ_ACTION_RECEIVE,          // Receive action  
			&msgprops,                  // Message property structure  
			NULL,                       // No OVERLAPPED structure  
			NULL,                       // No callback function  
			NULL,                       // No cursor handle  
			MQ_NO_TRANSACTION);			// Not in a transaction  

		MQCloseQueue(hQueue);
	}

	if (MQ_OK == hRet)return 0;
	return (-1);
}

//*****************************************************************************
// 函数名称: NewMutex
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
// 作	  者: sUesTcxF
//*****************************************************************************
int NewMutex(HANDLE pMutex)
{
	pMutex = CreateMutex(NULL, FALSE, NULL);
	return (*(int *)pMutex != 0) ? 0 : 1;
}

//*****************************************************************************
// 函数名称: GetMutex
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
// 作	  者: sUesTcxF
//*****************************************************************************
signed int GetMutex(HANDLE pMutex)
{
	return WaitForSingleObject(pMutex, INFINITE);
}

//*****************************************************************************
// 函数名称: RelMutex
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
// 作	  者: sUesTcxF
//*****************************************************************************
signed int RelMutex(HANDLE pMutex)
{
	return ReleaseMutex(pMutex);
}

void GetTime(TTime *pTime)
{
	SYSTEMTIME tTime;
	GetLocalTime(&tTime);
	pTime->wYear = tTime.wYear;
	pTime->bMonth = (BYTE)tTime.wMonth;
	pTime->bDay = (BYTE)tTime.wDay;
	pTime->bHour = (BYTE)tTime.wHour;
	pTime->bMinute = (BYTE)tTime.wMinute;
	pTime->bSecond = (BYTE)tTime.wSecond;
}

//*****************************************************************************
// 函数名称: GetClick
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
// 作	  者: sUesTcxF
//*****************************************************************************
unsigned int GetClick()
{
	//系统启动以来经过的毫秒数, 49天后溢出，在windows系上只作为调试用, 不需要修改
	unsigned int dwS = (unsigned int)(timeGetTime() / 1000);
	return dwS;
}

//*****************************************************************************
// 函数名称: GetTick
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
// 作	  者: sUesTcxF
//*****************************************************************************
unsigned int GetTick()
{
	//系统启动以来经过的毫秒数, 49天后溢出，在windows系统上只作为调试用, 不需要修改
	return timeGetTime();
}

//*****************************************************************************
// 函数名称: OpenComm
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
// 作	  者: sUesTcxF
//*****************************************************************************
//成功返回串口句柄 失败返回-1
signed int OpenComm(unsigned short wPort, unsigned int dwBaudRate, unsigned char bByteSize, unsigned char bStopBits, unsigned char bParity)
{
	bool fRet = true;
	char pszCom[32];
	memset(pszCom, 0, 32);
	memcpy(pszCom, "COM", strlen("COM"));

	//字符拼接
	char szPort[8];
	memset(szPort, 0, sizeof(szPort));
	_itoa_s((long)wPort, szPort, 10);
	memcpy(pszCom + strlen(pszCom), szPort, strlen(szPort));

	//转宽字符
	LPWSTR pszComPort = new WCHAR[256];
	if (NULL == pszComPort)return 0;
	memset(pszComPort, 0, sizeof(WCHAR) * 256);
	CharTowChar((const char *)pszCom, pszComPort, sizeof(wchar_t)*strlen(pszCom));

	HANDLE hHandle = CreateFile(pszComPort,						//串口号
		GENERIC_READ | GENERIC_WRITE,	//允许读和写
		0,								//独占方式
		NULL,							//
		OPEN_EXISTING,					//打开而不是创建
		0,								//同步方式
		NULL);							//

	if (pszComPort != NULL)
	{
		delete pszComPort;
		pszComPort = NULL;
	}

	if (hHandle == (HANDLE)-1)
	{
		TRACE("COMM-%d Open Fail.\n", wPort);
		return 0;
	}

	//设置波特率 奇偶校验
	DCB tDcb = { 0 };
	fRet = GetCommState(hHandle, &tDcb);
	if (!fRet)return 0;
	tDcb.BaudRate = dwBaudRate;
	tDcb.ByteSize = bByteSize;
	tDcb.StopBits = bStopBits;
	tDcb.Parity = bParity;
	SetCommState(hHandle, &tDcb);

	//输入缓冲区和输出缓冲区的大小都是1024
	fRet = SetupComm(hHandle, 100, 100);
	if (!fRet)return 0;
	//设定读写超时
	COMMTIMEOUTS tTimeOuts;
	tTimeOuts.ReadIntervalTimeout = MAXDWORD;
	tTimeOuts.ReadTotalTimeoutMultiplier = 0;
	tTimeOuts.ReadTotalTimeoutConstant = 0;
	//在读一次输入缓冲区的内容后读操作就立即返回，
	//而不管是否读入了要求的字符。

	//设定写超时
	tTimeOuts.WriteTotalTimeoutMultiplier = 100;
	tTimeOuts.WriteTotalTimeoutConstant = 500;
	fRet = SetCommTimeouts(hHandle, &tTimeOuts);
	if (!fRet)return 0;

	//清空缓冲区
	PurgeComm(hHandle, PURGE_TXCLEAR | PURGE_RXCLEAR);

	return (int)hHandle;
}

//*****************************************************************************
// 函数名称: CloseComm
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
// 作	  者: sUesTcxF
//*****************************************************************************
bool CloseComm(int hHandle)
{
	return CloseHandle((HANDLE)hHandle);
}

//*****************************************************************************
// 函数名称: CommReceive
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
// 作	  者: sUesTcxF
//*****************************************************************************
unsigned int CommReceive(int hHanlde, unsigned char *pBuf, unsigned short wBufSize, int *error)
{
	bool fRet = true;
	unsigned int dwReadSize = 0;
	fRet = ReadFile((HANDLE)hHanlde, pBuf, wBufSize, (LPDWORD)&dwReadSize, NULL);
	
	if (fRet)
	{
		return dwReadSize;
	}
	else
	{
		return 0;
	}
}

//*****************************************************************************
// 函数名称: CommSend
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
// 作	  者: sUesTcxF
//*****************************************************************************
bool CommSend(int hHanlde, unsigned char *pBuf, unsigned short wBufSize)
{
	bool fRet = true;
	unsigned int dwSendSize = 0;
	fRet = WriteFile((HANDLE)hHanlde, pBuf, wBufSize, (LPDWORD)&dwSendSize, NULL);
	if ((wBufSize == dwSendSize) && (fRet))
		return true;
	else
		return false;
}

//*****************************************************************************
// 函数名称: CloseSocket
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
// 作	  者: sUesTcxF
//*****************************************************************************
signed int CloseSocket(int *s)
{
	return closesocket(*s);
}

//*****************************************************************************
// 函数名称: ConvertAddr2IP
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
// 作	  者: sUesTcxF
//*****************************************************************************
//点分10进制转换为2进制
unsigned long ConvertAddr2IP(const char *pAddr)
{
	return inet_addr(pAddr);
}

//*****************************************************************************
// 函数名称: SocketShutDown
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
// 作	  者: sUesTcxF
//*****************************************************************************
signed int SocketShutDown(SOCKET *s)
{
	return shutdown(*s, 0);
}

//*****************************************************************************
// 函数名称: SetSocketNoBlock
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
// 作	  者: sUesTcxF
//*****************************************************************************
void SetSocketNoBlock(SOCKET Socket)
{
	;
}


void AppUpGrade()
{
	;
}

void SystemReboot()
{
	;
}

