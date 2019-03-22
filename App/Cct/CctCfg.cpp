//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : CctCfg.cpp
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
#include "CctCfg.h"
#include <ctype.h>


//*****************************************************************************
// 函数名称: GetDbPara
// 功能描述: 获取创建数据库的参数
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
void GetDbPara(TCctDbPara *pDbpara)
{
	const char *pDbName = "CctData.db";
	char szPath[128] = { 0 };

	pDbpara->wMtrNum = POINT_NUM;
	sprintf(szPath, (const char *)CCT_FILE_PATH"%s", (char *)pDbName);
	memcpy(pDbpara->szDbName, szPath, strlen(szPath));
}

//*****************************************************************************
// 函数名称: LoadCctPara
// 功能描述: 路由参数初始化
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
void LoadCctPara(char bLink, TCctPara *pCctPara)
{

	//pCctPara->RdrPara.bMtrPro = CctGetMtrPro();		//电表协议协议版本:CCT_MTRPRO_97/CCT_MTRPRO_07,取集抄全局的电表协议
	pCctPara->RdrPara.bMtrPro = CCT_MTRPROTO_07;		//默认07

	pCctPara->RdrPara.wMinRdTimes = 2;					//分钟选抄每一轮的尝试次数

	pCctPara->RdrPara.wHourRdTimes = 2;					//小时选抄每一轮的尝试次数
	pCctPara->RdrPara.dwHourRerdInterv = 30;			//小时补抄间隔,0表示只尝试1轮不进行补抄,单位分钟

	pCctPara->RdrPara.bHourDelay = 1; 					//小时抄读延迟时间,单位分钟,主用用来避免终端时间比电表快时,抄读电表冻结的时标不正确,终端延迟一段时间再去抄读
	pCctPara->RdrPara.bDayFrzDelay = 1;					//日冻结抄读延迟时间,单位分钟,主用用来避免终端时间比电表快

	pCctPara->RdrPara.wInfoPlcPara = INFO_PLC_PARA;								//载波参数更改
	pCctPara->RdrPara.wInfoPlcClrRt = INFO_PLC_CLRRT; 							//清路由
	pCctPara->RdrPara.wInfoPlcStopRd = INFO_PLC_STOPRD;							//停止抄表
	pCctPara->RdrPara.wInfoPlcResumeRd = INFO_PLC_RESUMERD;						//恢复抄表
	pCctPara->RdrPara.wInfoPlcRdAllRtInfo = INFO_PLC_RDALLRTINFO;				//读取所有节点的中继路由信息
	pCctPara->RdrPara.wInfoUpdateRT = INFO_PLC_UPDATE_ROUTE;					//载波路由器升级
	pCctPara->RdrPara.wInfoChannelChange = INFO_PLC_WIRLESS_CHANGE;      		//通信参数变更


	pCctPara->RdrPara.fUseLoopBuf = true;										//是否使用循环缓冲区
	pCctPara->RdrPara.wLoopBufSize = CCT_LOOPBUF_SIZE;							//循环缓冲区的大小

	memset(&pCctPara->RdrPara.bMainAddr, 0, sizeof(pCctPara->RdrPara.bMainAddr));//主节点地址
	//填写主节点地址 行政区划码+终端地址

	pCctPara->wFrmLenBytes = 2;
	pCctPara->bLink = bLink;

	unsigned char bRunMode = 1;
	pCctPara->bRunMode = bRunMode;												//路由版本 1、2、3

	pCctPara->RdrPara.bLogicPort = PORT_CCT_PLC;								//逻辑端口号,指的是通信协议上给每个通道规定的端口号,而不是物理端口号
	pCctPara->RdrPara.bPhyPort = COMM_CCT_PLC;									//物理端口号

	LoadCctPlcCom(&pCctPara->RdrPara.tCommPara);								//串口参数

}



//*****************************************************************************
// 函数名称: LoadCctPlcCom
// 功能描述: 装载PLC串口
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
void LoadCctPlcCom(TCommPara *pCommPara)
{
	pCommPara->wPort = CCT_PLC_COMM;
	pCommPara->dwBaudRate = CBR_115200;
	pCommPara->bByteSize = 8;
	pCommPara->bStopBits = ONESTOPBIT;
	pCommPara->bParity = NOPARITY;
}

//*****************************************************************************
// 函数名称: GetMtrAddr
// 功能描述: 获取645帧当中的电表地址
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
unsigned char *GetMtrAddr(unsigned char *pFrmData)
{
	if (NULL != pFrmData)
		return pFrmData + 1;

	return NULL;
}


//*****************************************************************************
// 函数名称: IsBufferAllZero
// 功能描述: 判断缓冲指是否全为0
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
bool IsBufferAllZero(unsigned char *pbBuf, unsigned short wLen)
{
	for (unsigned short i = 0; i < wLen; i++)
	{
		if (pbBuf[i])
			return false;
	}

	return true;
}

//*****************************************************************************
// 函数名称: IsBufferAllVal
// 功能描述: 判断缓冲指是否全为bVal
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
bool IsBufferAllVal(unsigned char *pbBuf, unsigned short wLen, unsigned char bVal)
{
	for (unsigned short i = 0; i < wLen; i++)
	{
		if (pbBuf[i] != bVal)
			return false;
	}

	return true;
}


//*****************************************************************************
// 函数名称: IsBufferAllEE
// 功能描述: 判断缓冲指是否全为0xee
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
bool IsBufferAllEE(unsigned char *pbBuf, unsigned short wLen)
{
	for (unsigned short i = 0; i < wLen; i++)
	{
		if (pbBuf[i] != 0xee)
			return false;
	}

	return true;
}

//*****************************************************************************
// 函数名称: GetMtrProtocol
// 功能描述: 获取电表协议版本
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
unsigned char GetMtrProtocol(unsigned char * pAddr)
{
	return CCT_MTRPROTO_07;
}

//*****************************************************************************
// 函数名称: GetDataID
// 功能描述: 获取645帧中的数据项ID
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
unsigned int GetDataID(unsigned char *pFrmData, unsigned char bMtrProtocol)
{
	if (pFrmData == NULL)
		return 0;

	if (bMtrProtocol == CCT_MTRPROTO_07)
		return *(unsigned int *)(pFrmData + 11);
	else
		return *(unsigned short *)(pFrmData +11);
}

//*****************************************************************************
// 函数名称: GetDataLen
// 功能描述: 获取645帧中的数据长度
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
unsigned char GetDataLen(unsigned char *pFrmData, unsigned char bMtrProtocol)
{
	if (pFrmData == NULL)
		return 0;

	if (bMtrProtocol == CCT_MTRPROTO_07)
		return (unsigned char)(*pFrmData + 9 - 4);
	else
		return (unsigned char)(*pFrmData + 9 - 4);
}

//*****************************************************************************
// 函数名称: LoadSocketPara
// 功能描述: 装载联网参数
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
void LoadSocketPara(TSocketPara *pSocketPara)
{
	//unsigned char bIP[4] = {192,168,3,45};
	//unsigned char bIP[4] = {192,168,1,121};
	//unsigned char bIP[4] = {111,230,145,58};
	unsigned char bIP[4] = {134,175,86,32};
	//unsigned char bIP[4] = {127,0,0,1};
	memcpy(pSocketPara->bIP, bIP, sizeof(bIP));
	pSocketPara->dwPort = 20001;
}



//*****************************************************************************
// 函数名称: IsInReadPeriod
// 功能描述: 判断是否属于抄表时段
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
bool IsInReadPeriod()
{
	return true;
}

//*****************************************************************************
// 函数名称: GetPlcPnMask
// 功能描述: 获取PLC测量点的Mask
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
const char* GetPlcPnMask()
{
	return NULL;
}

//*****************************************************************************
// 函数名称: QueryItemReadStatus
// 功能描述: 查询未抄读项
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
signed int QueryItemReadStatus()
{
	return 0;
}

//*****************************************************************************
// 函数名称: SearchPnFromMask
// 功能描述: 抄读小时曲线数据
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
signed short SearchPnFromMask(const char *pbPnMask, unsigned short wPn)
{

	return 0;
}

//*****************************************************************************
// 函数名称: SearchDoUnReadID
// 功能描述: 查询未抄读项ID
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
signed short SearchDoUnReadID(short wPn, char bClass, char bLinkType, TCctRwItem *pRwIte)
{
	//发送读取数据库消息
	return 0;
}

//*****************************************************************************
// 函数名称: TimeToSeconds
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
unsigned int TimeToSeconds(const TTime& time)
{
	return DaysFrom2000(time)*60*60*24 + (unsigned int )(time.bHour)*60*60 + (unsigned int )time.bMinute*60 + time.bSecond;
}


#define BASETIME          1999
#define BASEWEEK		  5			//1999/1/1是星期5 用5表示

//1、2、3、4、5、6、7、8、9、10、11、12
unsigned short g_wDaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//*****************************************************************************
// 函数名称: DaysFrom2000
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
//描述：2000/1/1到year/month/day的天数
unsigned int DaysFrom2000(const TTime& time)
{
	if (IsTimeEmpty(time))
	{
		return 0;
	}
	
	int y, m, d, days;
	y=BASETIME;
	m=1;
	d=1;

	days = 0;
	while (y < time.wYear)
	{
		if ((y%4==0 && y%100!=0) || y%400==0)   //闰年
			days += 366;
		else
			days += 365;

		y++;
	}

	//现在：y == year
	while (m < time.bMonth)
	{
		if (m == 2)
		{
			if ((y%4==0 && y%100!=0) || y%400==0)   //闰年
				days += 29;
			else
				days += 28;

		}
		else
		{
			days += g_wDaysOfMonth[m-1];
		}

		m++;
	}

	//现在：m == month
	days += time.bDay - d;

	return days;
}

//*****************************************************************************
// 函数名称: SecondsToTime
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
void SecondsToTime(unsigned int dwSeconds, TTime* pTime)
{
	int year=BASETIME, month=1, day=1, hour=0, minute=0, second=0;
	unsigned int delta;

	while (dwSeconds > 0)
	{
		if ((year%4==0 && year%100!=0) || year%400==0)   //闰年
			delta = 366*24*60*60;
		else
			delta = 365*24*60*60;

		if (dwSeconds < delta)
		{
			break;
		}
		else
		{
			dwSeconds -= delta;
			year++;
		}
	}
	

	while (dwSeconds > 0)
	{
		if (month == 2)
		{
			if ((year%4==0 && year%100!=0) || year%400==0)   //闰年
				delta = 29*24*60*60;
			else
				delta = 28*24*60*60;
		}
		else
		{
			delta = g_wDaysOfMonth[month-1]*24*60*60;
		}
		
		if (dwSeconds < delta)
		{
			break;
		}
		else
		{
			dwSeconds -= delta;
			month++;
		}
	}

	if (dwSeconds > 0)
	{
		day = dwSeconds / (24*60*60);
		
		dwSeconds -= day * 24 * 60 * 60;
		
		day++;
		
		if (dwSeconds > 0)
		{
			hour = dwSeconds / (60*60);
			
			dwSeconds -= hour * 60 * 60;
			
			if (dwSeconds > 0)
			{
				minute = dwSeconds / 60;
				second = dwSeconds - minute * 60;
			}
		}
	}
	
	pTime->wYear = year;
	pTime->bMonth = month;
	pTime->bDay = day;
	pTime->bHour = hour;
	pTime->bMinute = minute;
	pTime->bSecond = second;
	
}

//*****************************************************************************
// 函数名称: IsTimeEmpty
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
bool IsTimeEmpty(const TTime& time)
{
	if (time.wYear==0 && time.bMonth==0 && time.bDay==0 && 
		time.bHour==0 && time.bMinute==0 && time.bSecond==0)
		return true;
	else
		return false;
}


//*****************************************************************************
// 函数名称: HexToStr
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
void HexToStr(char *pbDest, char *pbSrc, unsigned int nLen)
{
	unsigned int i;
	char ddl,ddh;

	for (i=0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}

	pbDest[nLen*2] = '\0';
}

//*****************************************************************************
// 函数名称: StrToHex
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
void StrToHex(char *pbDest, char *pbSrc, unsigned int nLen)
{
	char h1,h2;
	char s1,s2;
	unsigned int i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
		{
			s1 -= 7;
		}

		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
		{
			s2 -= 7;
		}

		pbDest[i] = s1*16 + s2;
	}
	
}


//*****************************************************************************
// 函数名称: IsBcdCode
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
bool IsBcdCode(char *p, unsigned short num)
{
	char ch;
	for( int i=0; i<num; i++)
	{
		ch = p[i]&0xf;
		if (ch > 9)
			return false;

		ch = p[i]>>4;
		if (ch > 9)
			return false;
	}
	return true;
}

//*****************************************************************************
// 函数名称: BcdToByte
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
char BcdToByte(char bcd) 
{
	return ((bcd >> 4) & 0x0f) * 10 + (bcd & 0x0f);
}

//*****************************************************************************
// 函数名称: ByteToBcd
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
char ByteToBcd(char b)
{
	return (b/10<<4) + b%10;
}

//*****************************************************************************
// 函数名称: BcdToDWORD
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
unsigned int BcdToDWORD(char* p, unsigned short len)
{
	char* p0 = p;
	p += len - 1;

	unsigned int val = 0;
	for (; p>=p0; p--)
	{
		val = val * 100 + ((*p >> 4) & 0x0f) * 10 + (*p & 0x0f);
	}

	return val;
}

//*****************************************************************************
// 函数名称: BcdToWORD
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
unsigned short BcdToWORD(char * p) 
{
	char* p0 = p;
	p0 += 1;

	unsigned short val = 0;
	for (; p<=p0; p++)
	{
		val = val * 100 + ((*p >> 4) & 0x0f) * 10 + (*p & 0x0f);
	}

	return val;
}

//*****************************************************************************
// 函数名称: BcdToDDWORD
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
unsigned long long BcdToDDWORD(char* p, unsigned short len)
{
	char* p0 = p;
	p += len - 1;

	unsigned long long val = 0;
	for (; p>=p0; p--)
	{
		val = val * 100 + ((*p >> 4) & 0x0f) * 10 + (*p & 0x0f);
	}

	return val; 
}

//*****************************************************************************
// 函数名称: ByteToWord
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
unsigned short ByteToWord(char* pbBuf, unsigned short wLen)
{
	unsigned int val = 0;
	if (wLen > 2)
		wLen = 2;

	memcpy(&val, pbBuf, wLen);
	return val;
}

//*****************************************************************************
// 函数名称: ByteToDWORD
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
unsigned int ByteToDWORD(char* pbBuf, unsigned short wLen)
{
	unsigned int val = 0;
	if (wLen > 4)
		wLen = 4;
		
	memcpy(&val, pbBuf, wLen);
	return val;
}

//*****************************************************************************
// 函数名称: BcdToUint64
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
unsigned long long BcdToUint64(char* p, unsigned short len)
{
	char* p0 = p;
	p += len - 1;

	unsigned long long val = 0;
	for (; p>=p0; p--)
	{
		val = val * 100 + ((*p >> 4) & 0x0f) * 10 + (*p & 0x0f);
	}

	return val;
}


//*****************************************************************************
// 函数名称: BcdToInt
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
int BcdToInt(char* p, unsigned short len)
{
	char* p0 = p;

	p += len - 1;
	int val = (*p & 0x0f);

	bool fNeg;
	if ((*p&0xf0) != 0)
		fNeg = true;
	else
		fNeg = false;

	p--;

	for (; p>=p0; p--)
	{
		val = val * 100 + ((*p >> 4) & 0x0f) * 10 + (*p & 0x0f);
	}

	if (fNeg)
	    return -val;
	else
		return val;
}

//*****************************************************************************
// 函数名称: WordToBcd
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
unsigned short WordToBcd(unsigned short num)
{
	unsigned short bcd;

	num = num % 10000;
	bcd = num/1000;
	num = num%1000;
	bcd <<= 4;
	bcd += num/100;
	num = num%100;
	bcd <<= 4;
	bcd += num/10;
	bcd <<= 4;
	bcd += num%10;
	return bcd;
}

//*****************************************************************************
// 函数名称: IntToBCD
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
void IntToBCD(int val, char* bcd, unsigned short len)
{
	bool fNeg = false;
	if (val < 0)
	{
		val = - val;
		fNeg = true;
	}

	int power = 1;
	for (unsigned short i=0; i<len-1; i++)
	{
		power *= 100; 
	}

	power *= 10;  
	val %= power;

	power /= 10;

	if (fNeg)
		bcd[len - 1] = 0x10 + val / power;
	else
		bcd[len - 1] = val / power;

	len--;

	for (; len>0; len--)
	{
		char bHigh, bLow;
	    val %= power;
	    power /= 10;
		bHigh = val / power;

	    val %= power;
	    power /= 10;
		bLow = val / power;
		bcd[len - 1] = (bHigh << 4) | bLow;
	}
}

//*****************************************************************************
// 函数名称: DWORDToBCD
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
void DWORDToBCD(unsigned int val, char* bcd, unsigned short len)
{
	unsigned int power;
	char bHigh, bLow;

	memset(bcd,0,len);
	if (len > 4)//处理最高字节
	{
		power = 1000000000;
		bHigh = (char )(val / power);
	    val %= power;

		power = 100000000;
		bLow = (char )(val / power);
	    val %= power;
		bcd[4] = (bHigh << 4) | bLow;

		len = 4;
	}

	power = 1;
	for (unsigned short i=0; i<len; i++)
	{
		power *= 100; 
	}

	for (; len>0; len--)
	{
	    val %= power;
	    power /= 10;
		bHigh = (char )(val / power);

	    val %= power;
	    power /= 10;
		bLow = (char )(val / power);
		bcd[len - 1] = (bHigh << 4) | bLow;
	}
}

//*****************************************************************************
// 函数名称: DWordToByte
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
void DWordToByte(unsigned int val, char* byte, unsigned short len)
{
	if( len > 4 ) len = 4;
	for(; len>0; byte++, len--)
	{
		*byte = (char)val%0x100;
		val = val/0x100;
	}
}

//*****************************************************************************
// 函数名称: DWordToByte
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
unsigned short DWordToByte(unsigned int dw, char* p)
{
	*p++ = dw & 0xff;
	*p++ = (dw >> 8) & 0xff;
	*p++ = (dw >> 16) & 0xff;
	*p = (dw >> 24) & 0xff;
	return 4;
}

//*****************************************************************************
// 函数名称: ByteToDWord
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
unsigned int ByteToDWord(char* p)
{
	unsigned int dw = 0;
	memcpy(&dw, p, 4);
	return dw;
	//return *p + (DWORD )*(p + 1)*0x100 + (DWORD )*(p + 2)*0x10000 + (DWORD )*(p + 3)*0x1000000;
}

//*****************************************************************************
// 函数名称: Uint64ToBCD
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
void Uint64ToBCD(unsigned long long val, char* bcd, unsigned short len)
{
	unsigned long long power;
	char bHigh, bLow;

	memset(bcd, 0, len);

	power = 1;
	for (unsigned short i=0; i<len; i++)
	{
		power *= 100; 
	}

	for (; len>0; len--)
	{
	    val %= power;
	    power /= 10;
		bHigh = (char )(val / power);

	    val %= power;
	    power /= 10;
		bLow = (char )(val / power);
		bcd[len - 1] = (bHigh << 4) | bLow;
	}
}

//*****************************************************************************
// 函数名称: CheckSum
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
unsigned char CheckSum(unsigned char* p, unsigned short wLen)
{
    unsigned char bSum = 0;	
    for (; wLen > 0; wLen--)
	{
  	    bSum += *p++;
	}	
    return bSum;
}


