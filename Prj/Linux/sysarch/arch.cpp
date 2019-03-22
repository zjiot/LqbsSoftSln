//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : arch.cpp
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 操作系统差异接口
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************


#include "arch.h"
#include "MsgQueue.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <termios.h>
#include <errno.h>

//*****************************************************************************
// 函数名称: Sleep
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
void Sleep(unsigned int dwMs)
{
	usleep(dwMs*1000);
}

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
void NewTask(TTaskRet (* pFunc)(void *arg), void *pArg, unsigned int nStackSize, TPrio prio)
{

	pthread_t dwThrId = 0;
	pthread_create(&dwThrId, NULL, (TFunc)pFunc, NULL);
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
signed int NewQueue(unsigned int dwQueueId, char *pszName,  unsigned short dwMsgArrayCnt)
{
	SYS_MSG_BUF tMsgBuf;
	memset(&tMsgBuf, 0, sizeof(SYS_MSG_BUF));

	//clear msgqueue
	while(!ClearQueue(SYS_QUEUE_ID_BASE + dwQueueId));
	
	return SYS_QUEUE_ID_BASE + dwQueueId;
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
//成功返回0 失败返回错误码
signed int SendMessage2Queue(unsigned int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, unsigned int dwTimeOut)
{
	SYS_MSG_BUF tMsgBuf;
	signed int dwRet = (-1);
	struct msqid_ds buf;
	unsigned char times = 0;
	

	//ftok()得到
	signed int dwFtokHandle = 0;		
	signed int dwQueuHandle = 0;
	memset(&tMsgBuf, 0, sizeof(SYS_MSG_BUF));


	if (pstMsg == NULL)
	{
		TRACE("SendMessage2Queue Fail, pstMsg NULL.\n");
	}
			
	dwFtokHandle = GetQueueHandle((APP_QUEUE_ID_ENUM)dwQueueId);
	//TRACE("SendMessage2Queue dwFtokHandle %d.\n", dwFtokHandle);
	
	dwQueuHandle = msgget(dwFtokHandle, IPC_CREAT);
	
	while(1)
	{
		if ((-1) != msgctl(dwQueuHandle, IPC_STAT, &buf))
		{
			if (buf.msg_cbytes-buf.msg_qbytes > sizeof(CCT_QUEUE_MSG_S))
			{
				break;
			}

			Sleep(10);
			times++;
			if (times > 5)	
			{
				return (-1);
			}	
		}	
		else
		{
			return (-1);
		}
	}
	
	//TRACE("SendMessage2Queue dwQueuwdle %d.\n", dwQueuHandle);
	
	if (dwQueuHandle > 0)
	{
		tMsgBuf.type = 1;
		memcpy(tMsgBuf.bData, pstMsg, sizeof(CCT_QUEUE_MSG_S));

		dwRet = msgsnd(dwQueuHandle, &tMsgBuf, sizeof(CCT_QUEUE_MSG_S), IPC_NOWAIT);
		//TRACE("SendMessage2Queue dwRet %d.\n", dwRet);
	}
	
	if (dwRet == 0)
	{
		return 0;
	}
	else
	{
		return (-1);
	}
	
}

//*****************************************************************************
// 函数名称: ClearQueue
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
signed int ClearQueue(signed int dwFtokHandle)
{
	signed int dwRet = (-1);
	SYS_MSG_BUF tMsgBuf;
	signed int dwQueuHandle = 0;
	memset(&tMsgBuf, 0, sizeof(SYS_MSG_BUF));
	
	dwQueuHandle = msgget(dwFtokHandle, IPC_CREAT);
	if (dwQueuHandle > 0)
	{
		dwRet = msgrcv(dwQueuHandle, &tMsgBuf, MAX_MSG_LEN, 1, IPC_NOWAIT);
		if (dwRet > 0)
		{
			return 0;
		}
		else
		{
			return (-1);
		}
	}
	else
	{
		return (-1);
	}
	
	//TRACE("WaitMessage dwRet %d.\n", dwRet);
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
//成功返回0 失败返回错误码
signed int WaitMessage(unsigned int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, unsigned int dwTimeOut)
{
	signed int dwRet = (-1);
	signed int dwFtokHandle = 0;		
	signed int dwQueuHandle = 0;
	SYS_MSG_BUF tMsgBuf;
	unsigned int dwMsgflg = 0;

	//TRACE("WaitMessage dwQueueId %d.\n", dwQueueId);

	memset(&tMsgBuf, 0, sizeof(SYS_MSG_BUF));

	if (pstMsg == NULL)
	{
		TRACE("WaitMessage Fail, pstMsg NULL.\n");
	}
	
	dwFtokHandle = GetQueueHandle((APP_QUEUE_ID_ENUM)dwQueueId);
	//TRACE("WaitMessage dwFtokHandle %d.\n", dwFtokHandle);
	
	dwQueuHandle = msgget(dwFtokHandle, IPC_CREAT);
	//TRACE("WaitMessage dwQueuHandle %d.\n", dwQueuHandle);
	
	if (dwQueuHandle > 0)
	{
		if (0xffffffff == dwTimeOut)//阻塞
			dwMsgflg = 0;
		else
			dwMsgflg = IPC_NOWAIT;//非阻塞
		
		dwRet = msgrcv(dwQueuHandle, &tMsgBuf, MAX_MSG_LEN, 0, dwMsgflg);
	}
	
	//TRACE("WaitMessage dwRet %d.\n", dwRet);

	if (dwRet > 0)
	{
		memcpy(pstMsg, tMsgBuf.bData, sizeof(CCT_QUEUE_MSG_S));
		return 0;
	}
	else
	{
		return (-1);
	}

}

//*****************************************************************************
// 函数名称: CreateMsgQueueueFile
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
bool CreateMsgQueueueFile(const char *pszName)
{
	char pszRmPathName[32] = {0};

	//先删除
	sprintf(pszRmPathName, "%s", "rm -rf ");
	sprintf(pszRmPathName+strlen("rm -rf "), "%s", pszName);
	system(pszRmPathName);
	int fd = open(pszName, O_CREAT|O_RDWR, 0777);
	if(fd == -1)
	{
		TRACE("Create MsgQueue File %s Fail.\n", pszName);
		return false;
	}

	close(fd);
	system("chmod -R 777 ../");

	return true;	
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
signed int NewMutex(pthread_mutex_t *pMutex)
{
	if (pMutex == NULL)
	{
		return (-1);
	}

	return pthread_mutex_init(pMutex, NULL);
}

//*****************************************************************************
// 函数名称: DelMutex
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
signed int DelMutex(pthread_mutex_t *pMutex)
{
	if (pMutex == NULL)
	{
		return (-1);
	}

	return pthread_mutex_destroy(pMutex);
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
signed int GetMutex(pthread_mutex_t *pMutex)
{
	if (pMutex == NULL)
	{
		return (-1);
	}

	return pthread_mutex_lock(pMutex);
}

//*****************************************************************************
// 函数名称: GetMutexTry
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
signed int GetMutexTry(pthread_mutex_t *pMutex)
{
	if (pMutex == NULL)
	{
		return (-1);
	}

	return pthread_mutex_trylock(pMutex);
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
signed int RelMutex(pthread_mutex_t *pMutex)
{
	if (pMutex == NULL)
	{
		return (-1);
	}

	return pthread_mutex_unlock(pMutex);
}

//*****************************************************************************
// 函数名称: GetTime
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
void GetTime(TTime *pTime)
{
	if (NULL == pTime)
	{
		return;
	}
	
    time_t nSeconds;
    struct tm *pTM;
    
    time(&nSeconds);
    pTM = localtime(&nSeconds);

	pTime->wYear =		pTM->tm_year + 1900;
	pTime->bMonth =		pTM->tm_mon + 1;
	pTime->bDay =		pTM->tm_mday;
	pTime->bHour =		pTM->tm_hour;
	pTime->bMinute =	pTM->tm_min;
	pTime->bSecond =	pTM->tm_sec;
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
	//其中，cld_id类型四种：   
	//a、CLOCK_REALTIME:系统实时时间,随系统实时时间改变而改变
	//b、CLOCK_MONOTONIC,从系统启动这一刻起开始计时,不受系统时间被用户改变的影响(tTime.tv_nsec-136年后溢出)
	//c、CLOCK_PROCESS_CPUTIME_ID,本进程到当前代码系统CPU花费的时间
	//d、CLOCK_THREAD_CPUTIME_ID,本线程到当前代码系统CPU花费的时间

	//struct timespec {
	//time_t tv_sec; /* 秒*/
	//long tv_nsec; /* 纳秒*/
	//};
	
	struct timespec tTime;
	clock_gettime(CLOCK_MONOTONIC, &tTime);
	return tTime.tv_sec;
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
	//其中，cld_id类型四种：   
	//a、CLOCK_REALTIME:系统实时时间,随系统实时时间改变而改变
	//b、CLOCK_MONOTONIC,从系统启动这一刻起开始计时,不受系统时间被用户改变的影响(tTime.tv_nsec-136年后溢出)
	//c、CLOCK_PROCESS_CPUTIME_ID,本进程到当前代码系统CPU花费的时间
	//d、CLOCK_THREAD_CPUTIME_ID,本线程到当前代码系统CPU花费的时间

	//struct timespec {
	//time_t tv_sec; /* 秒*/
	//long tv_nsec; /* 纳秒*/
	//};
	
	struct timespec tTime;
	clock_gettime(CLOCK_MONOTONIC, &tTime);
	return tTime.tv_nsec/1000000;
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
signed int OpenComm(unsigned short wPort, unsigned int dwBaudRate, unsigned char bByteSize, unsigned char bStopBits, unsigned char bParity)
{
	TRACE("CommIf::OpenComm wPort %d.\n", wPort);
	
	char pszPort[64];
	memset(pszPort, 0, sizeof(pszPort));
	sprintf(pszPort, "%s%d", "/dev/ttyS", wPort);
	TRACE("CommIf::OpenComm pszPort %s.\n", pszPort);


	signed int fd = open(pszPort, O_RDWR);
	if (fd == -1)return 0;

	if (!SetCommPara(fd, dwBaudRate, bByteSize, bStopBits, bParity))
	{
		TRACE("CommIf::SetCommPara Fail wPort %d.\n", wPort);
	}
	
	return fd;
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
unsigned int CommReceive(signed int hHanlde, unsigned char *pBuf, unsigned short wBufSize, int *error)
{
	signed int dwWSize = 0;

	if(hHanlde<=0 || pBuf==NULL || wBufSize<=0)
	{
		TRACE("CommReceive err. param error. hHanlde[%d], wBufSize[%d]\n", hHanlde, wBufSize);
		return 0;
	}
	
	dwWSize = read(hHanlde, pBuf, wBufSize);
	if (dwWSize >= 0)
	{
		return dwWSize;
	}
	else
	{
		TRACE("CommReceive err. dwWSize[%d],err_no[%d]\n", dwWSize, errno);
		if(error != NULL)
		{
			*error = -1;
		}
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
bool CommSend(signed int hHanlde, unsigned char *pBuf, unsigned short wBufSize)
{
	signed int dwRSize = 0;
	dwRSize = write(hHanlde, pBuf, wBufSize);
	if (wBufSize == dwRSize)return true;
	return false;
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
bool CloseComm(signed int hHandle)
{
	close(hHandle);
	return true;
}

//*****************************************************************************
// 函数名称: SetCommPara
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
bool SetCommPara(unsigned int fd, unsigned int bps, unsigned int databits, unsigned int stopbits, unsigned int parity)
{
	TRACE("CommIf::SetCommPara fd %d.\n", fd);

	struct termios options; 
	if(tcgetattr(fd,&options) != 0)
	{
		return(false);  
	}

	tcgetattr(fd, &options);
	switch(bps)
	{
		case 300:
			cfsetispeed(&options,B300);
			cfsetospeed(&options,B300);			
			break;	
		case 600:
			cfsetispeed(&options,B600);
			cfsetospeed(&options,B600);
			break;	
		case 1200:
			cfsetispeed(&options,B1200);
			cfsetospeed(&options,B1200);
			break;
		case 2400:
			cfsetispeed(&options,B2400);
			cfsetospeed(&options,B2400);
			break;
		case 4800:
			cfsetispeed(&options,B4800);
			cfsetospeed(&options,B4800);
			break;
		case 9600:
			cfsetispeed(&options,B9600);
			cfsetospeed(&options,B9600);
			break;
		case 19200:
			cfsetispeed(&options,B19200);
			cfsetospeed(&options,B19200);
			break;
		case 38400:
			cfsetispeed(&options,B38400);
			cfsetospeed(&options,B38400);
			break;
		case 57600:
			cfsetispeed(&options,B57600);
			cfsetospeed(&options,B57600);
			break;
		case 115200:
			cfsetispeed(&options,B115200);
			cfsetospeed(&options,B115200);
			break;
		default:
			cfsetispeed(&options,B1200);
	}
//	tcsetattr(fd,TCANOW,&Opt);	


	options.c_cflag &= ~CSIZE; 
	switch (databits) 
	{
		case 7:
			options.c_cflag |= CS7; 
			break;
		case 8:     
			options.c_cflag |= CS8;
			break;   
		default:    
			return (false);
	}
	
	options.c_iflag &= ~ICRNL;
	options.c_iflag &= ~IXON;
	options.c_iflag &= ~IXOFF;
	
	switch (parity) 
	{   
		case 0:    
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
			break;  
		case 1:     
			options.c_cflag |= (PARODD | PARENB); 
			options.c_iflag |= INPCK;             /* Disnable parity checking */ 
			break;  
		case 2:   
			options.c_cflag |= PARENB;     /* Enable parity */    
			options.c_cflag &= ~PARODD;  
//			options.c_iflag |= INPCK;       /* Disnable parity checking */
			options.c_iflag |= IGNBRK;       /* Disnable parity checking */
			break;  
		default:   
			return (false);  
	}  

	switch (stopbits)
	{   
		//case 1:    
		case 0:  
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
			break;
		default:    
			return (false); 
	} 

/* Set input parity option */ 
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	options.c_oflag  &= ~OPOST;   /*Output*/

	if(parity != 0)
	{
		options.c_iflag |= INPCK; 
	}
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 1;  
	options.c_cc[VMIN] = 0;  
	if(tcsetattr(fd,TCSANOW,&options) != 0)   
	{ 
		return (false);  
	} 
	
	return (true);  
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
signed int CloseSocket(signed int *s)
{
	return close(*s);
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
	return shutdown(*s, SHUT_RDWR);
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
	int oldOption = fcntl(Socket, F_GETFL);
	int newOption = oldOption | O_NONBLOCK;
	//设置sockfd非阻塞
	fcntl(Socket, F_SETFL, newOption);
}

//*****************************************************************************
// 函数名称: sprintf_s
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
int sprintf_s(char *buffer, const char *format, char * src, ...)
{
	//return snprintf(buffer, strlen(src), src);
}


void AppUpGrade()
{
	system("chmod 777 /usr/update.sh");	
	system("/usr/update.sh");	
}

void SystemReboot()
{
	system("reboot");	
}

