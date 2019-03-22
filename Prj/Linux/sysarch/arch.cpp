//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : arch.cpp
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : ����ϵͳ����ӿ�
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
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
// ��������: Sleep
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
// ��	  ��: sUesTcxF
//*****************************************************************************
void Sleep(unsigned int dwMs)
{
	usleep(dwMs*1000);
}

//*****************************************************************************
// ��������: NewTask
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
// ��	  ��: sUesTcxF
//*****************************************************************************
void NewTask(TTaskRet (* pFunc)(void *arg), void *pArg, unsigned int nStackSize, TPrio prio)
{

	pthread_t dwThrId = 0;
	pthread_create(&dwThrId, NULL, (TFunc)pFunc, NULL);
}

//*****************************************************************************
// ��������: NewQueue
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
// ��	  ��: sUesTcxF
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
// ��������: SendMessage2Queue
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
// ��	  ��: sUesTcxF
//*****************************************************************************
//�ɹ�����0 ʧ�ܷ��ش�����
signed int SendMessage2Queue(unsigned int dwQueueId, CCT_QUEUE_MSG_S *pstMsg, unsigned int dwTimeOut)
{
	SYS_MSG_BUF tMsgBuf;
	signed int dwRet = (-1);
	struct msqid_ds buf;
	unsigned char times = 0;
	

	//ftok()�õ�
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
// ��������: ClearQueue
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
// ��	  ��: sUesTcxF
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
// ��������: WaitMessage
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
// ��	  ��: sUesTcxF
//*****************************************************************************
//�ɹ�����0 ʧ�ܷ��ش�����
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
		if (0xffffffff == dwTimeOut)//����
			dwMsgflg = 0;
		else
			dwMsgflg = IPC_NOWAIT;//������
		
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
// ��������: CreateMsgQueueueFile
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
// ��	  ��: sUesTcxF
//*****************************************************************************
bool CreateMsgQueueueFile(const char *pszName)
{
	char pszRmPathName[32] = {0};

	//��ɾ��
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
// ��������: NewMutex
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
// ��	  ��: sUesTcxF
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
// ��������: DelMutex
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
// ��	  ��: sUesTcxF
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
// ��������: GetMutex
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
// ��	  ��: sUesTcxF
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
// ��������: GetMutexTry
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
// ��	  ��: sUesTcxF
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
// ��������: RelMutex
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
// ��	  ��: sUesTcxF
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
// ��������: GetTime
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
// ��	  ��: sUesTcxF
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
// ��������: GetClick
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
// ��	  ��: sUesTcxF
//*****************************************************************************
unsigned int GetClick()
{
	//���У�cld_id�������֣�   
	//a��CLOCK_REALTIME:ϵͳʵʱʱ��,��ϵͳʵʱʱ��ı���ı�
	//b��CLOCK_MONOTONIC,��ϵͳ������һ����ʼ��ʱ,����ϵͳʱ�䱻�û��ı��Ӱ��(tTime.tv_nsec-136������)
	//c��CLOCK_PROCESS_CPUTIME_ID,�����̵���ǰ����ϵͳCPU���ѵ�ʱ��
	//d��CLOCK_THREAD_CPUTIME_ID,���̵߳���ǰ����ϵͳCPU���ѵ�ʱ��

	//struct timespec {
	//time_t tv_sec; /* ��*/
	//long tv_nsec; /* ����*/
	//};
	
	struct timespec tTime;
	clock_gettime(CLOCK_MONOTONIC, &tTime);
	return tTime.tv_sec;
}

//*****************************************************************************
// ��������: GetTick
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
// ��	  ��: sUesTcxF
//*****************************************************************************
unsigned int GetTick()
{
	//���У�cld_id�������֣�   
	//a��CLOCK_REALTIME:ϵͳʵʱʱ��,��ϵͳʵʱʱ��ı���ı�
	//b��CLOCK_MONOTONIC,��ϵͳ������һ����ʼ��ʱ,����ϵͳʱ�䱻�û��ı��Ӱ��(tTime.tv_nsec-136������)
	//c��CLOCK_PROCESS_CPUTIME_ID,�����̵���ǰ����ϵͳCPU���ѵ�ʱ��
	//d��CLOCK_THREAD_CPUTIME_ID,���̵߳���ǰ����ϵͳCPU���ѵ�ʱ��

	//struct timespec {
	//time_t tv_sec; /* ��*/
	//long tv_nsec; /* ����*/
	//};
	
	struct timespec tTime;
	clock_gettime(CLOCK_MONOTONIC, &tTime);
	return tTime.tv_nsec/1000000;
}


//*****************************************************************************
// ��������: OpenComm
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
// ��	  ��: sUesTcxF
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
// ��������: CommReceive
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
// ��	  ��: sUesTcxF
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
// ��������: CommSend
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
// ��	  ��: sUesTcxF
//*****************************************************************************
bool CommSend(signed int hHanlde, unsigned char *pBuf, unsigned short wBufSize)
{
	signed int dwRSize = 0;
	dwRSize = write(hHanlde, pBuf, wBufSize);
	if (wBufSize == dwRSize)return true;
	return false;
}

//*****************************************************************************
// ��������: CloseComm
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
// ��	  ��: sUesTcxF
//*****************************************************************************
bool CloseComm(signed int hHandle)
{
	close(hHandle);
	return true;
}

//*****************************************************************************
// ��������: SetCommPara
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
// ��	  ��: sUesTcxF
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
// ��������: CloseSocket
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
// ��	  ��: sUesTcxF
//*****************************************************************************
signed int CloseSocket(signed int *s)
{
	return close(*s);
}

//*****************************************************************************
// ��������: ConvertAddr2IP
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
// ��	  ��: sUesTcxF
//*****************************************************************************
unsigned long ConvertAddr2IP(const char *pAddr)
{
	return inet_addr(pAddr);
}

//*****************************************************************************
// ��������: SocketShutDown
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
// ��	  ��: sUesTcxF
//*****************************************************************************
signed int SocketShutDown(SOCKET *s)
{
	return shutdown(*s, SHUT_RDWR);
}

//*****************************************************************************
// ��������: SetSocketNoBlock
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
// ��	  ��: sUesTcxF
//*****************************************************************************
void SetSocketNoBlock(SOCKET Socket)
{
	int oldOption = fcntl(Socket, F_GETFL);
	int newOption = oldOption | O_NONBLOCK;
	//����sockfd������
	fcntl(Socket, F_SETFL, newOption);
}

//*****************************************************************************
// ��������: sprintf_s
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
// ��	  ��: sUesTcxF
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

