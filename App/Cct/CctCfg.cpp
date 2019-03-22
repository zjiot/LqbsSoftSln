//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : CctCfg.cpp
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
#include "CctCfg.h"
#include <ctype.h>


//*****************************************************************************
// ��������: GetDbPara
// ��������: ��ȡ�������ݿ�Ĳ���
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: LoadCctPara
// ��������: ·�ɲ�����ʼ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void LoadCctPara(char bLink, TCctPara *pCctPara)
{

	//pCctPara->RdrPara.bMtrPro = CctGetMtrPro();		//���Э��Э��汾:CCT_MTRPRO_97/CCT_MTRPRO_07,ȡ����ȫ�ֵĵ��Э��
	pCctPara->RdrPara.bMtrPro = CCT_MTRPROTO_07;		//Ĭ��07

	pCctPara->RdrPara.wMinRdTimes = 2;					//����ѡ��ÿһ�ֵĳ��Դ���

	pCctPara->RdrPara.wHourRdTimes = 2;					//Сʱѡ��ÿһ�ֵĳ��Դ���
	pCctPara->RdrPara.dwHourRerdInterv = 30;			//Сʱ�������,0��ʾֻ����1�ֲ����в���,��λ����

	pCctPara->RdrPara.bHourDelay = 1; 					//Сʱ�����ӳ�ʱ��,��λ����,�������������ն�ʱ��ȵ���ʱ,����������ʱ�겻��ȷ,�ն��ӳ�һ��ʱ����ȥ����
	pCctPara->RdrPara.bDayFrzDelay = 1;					//�ն��᳭���ӳ�ʱ��,��λ����,�������������ն�ʱ��ȵ���

	pCctPara->RdrPara.wInfoPlcPara = INFO_PLC_PARA;								//�ز���������
	pCctPara->RdrPara.wInfoPlcClrRt = INFO_PLC_CLRRT; 							//��·��
	pCctPara->RdrPara.wInfoPlcStopRd = INFO_PLC_STOPRD;							//ֹͣ����
	pCctPara->RdrPara.wInfoPlcResumeRd = INFO_PLC_RESUMERD;						//�ָ�����
	pCctPara->RdrPara.wInfoPlcRdAllRtInfo = INFO_PLC_RDALLRTINFO;				//��ȡ���нڵ���м�·����Ϣ
	pCctPara->RdrPara.wInfoUpdateRT = INFO_PLC_UPDATE_ROUTE;					//�ز�·��������
	pCctPara->RdrPara.wInfoChannelChange = INFO_PLC_WIRLESS_CHANGE;      		//ͨ�Ų������


	pCctPara->RdrPara.fUseLoopBuf = true;										//�Ƿ�ʹ��ѭ��������
	pCctPara->RdrPara.wLoopBufSize = CCT_LOOPBUF_SIZE;							//ѭ���������Ĵ�С

	memset(&pCctPara->RdrPara.bMainAddr, 0, sizeof(pCctPara->RdrPara.bMainAddr));//���ڵ��ַ
	//��д���ڵ��ַ ����������+�ն˵�ַ

	pCctPara->wFrmLenBytes = 2;
	pCctPara->bLink = bLink;

	unsigned char bRunMode = 1;
	pCctPara->bRunMode = bRunMode;												//·�ɰ汾 1��2��3

	pCctPara->RdrPara.bLogicPort = PORT_CCT_PLC;								//�߼��˿ں�,ָ����ͨ��Э���ϸ�ÿ��ͨ���涨�Ķ˿ں�,����������˿ں�
	pCctPara->RdrPara.bPhyPort = COMM_CCT_PLC;									//����˿ں�

	LoadCctPlcCom(&pCctPara->RdrPara.tCommPara);								//���ڲ���

}



//*****************************************************************************
// ��������: LoadCctPlcCom
// ��������: װ��PLC����
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: GetMtrAddr
// ��������: ��ȡ645֡���еĵ���ַ
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
unsigned char *GetMtrAddr(unsigned char *pFrmData)
{
	if (NULL != pFrmData)
		return pFrmData + 1;

	return NULL;
}


//*****************************************************************************
// ��������: IsBufferAllZero
// ��������: �жϻ���ָ�Ƿ�ȫΪ0
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: IsBufferAllVal
// ��������: �жϻ���ָ�Ƿ�ȫΪbVal
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: IsBufferAllEE
// ��������: �жϻ���ָ�Ƿ�ȫΪ0xee
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: GetMtrProtocol
// ��������: ��ȡ���Э��汾
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
unsigned char GetMtrProtocol(unsigned char * pAddr)
{
	return CCT_MTRPROTO_07;
}

//*****************************************************************************
// ��������: GetDataID
// ��������: ��ȡ645֡�е�������ID
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: GetDataLen
// ��������: ��ȡ645֡�е����ݳ���
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: LoadSocketPara
// ��������: װ����������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: IsInReadPeriod
// ��������: �ж��Ƿ����ڳ���ʱ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
bool IsInReadPeriod()
{
	return true;
}

//*****************************************************************************
// ��������: GetPlcPnMask
// ��������: ��ȡPLC�������Mask
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
const char* GetPlcPnMask()
{
	return NULL;
}

//*****************************************************************************
// ��������: QueryItemReadStatus
// ��������: ��ѯδ������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed int QueryItemReadStatus()
{
	return 0;
}

//*****************************************************************************
// ��������: SearchPnFromMask
// ��������: ����Сʱ��������
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed short SearchPnFromMask(const char *pbPnMask, unsigned short wPn)
{

	return 0;
}

//*****************************************************************************
// ��������: SearchDoUnReadID
// ��������: ��ѯδ������ID
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed short SearchDoUnReadID(short wPn, char bClass, char bLinkType, TCctRwItem *pRwIte)
{
	//���Ͷ�ȡ���ݿ���Ϣ
	return 0;
}

//*****************************************************************************
// ��������: TimeToSeconds
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
// ��    ��: sUesTcxF
//*****************************************************************************
unsigned int TimeToSeconds(const TTime& time)
{
	return DaysFrom2000(time)*60*60*24 + (unsigned int )(time.bHour)*60*60 + (unsigned int )time.bMinute*60 + time.bSecond;
}


#define BASETIME          1999
#define BASEWEEK		  5			//1999/1/1������5 ��5��ʾ

//1��2��3��4��5��6��7��8��9��10��11��12
unsigned short g_wDaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//*****************************************************************************
// ��������: DaysFrom2000
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
// ��    ��: sUesTcxF
//*****************************************************************************
//������2000/1/1��year/month/day������
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
		if ((y%4==0 && y%100!=0) || y%400==0)   //����
			days += 366;
		else
			days += 365;

		y++;
	}

	//���ڣ�y == year
	while (m < time.bMonth)
	{
		if (m == 2)
		{
			if ((y%4==0 && y%100!=0) || y%400==0)   //����
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

	//���ڣ�m == month
	days += time.bDay - d;

	return days;
}

//*****************************************************************************
// ��������: SecondsToTime
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
// ��    ��: sUesTcxF
//*****************************************************************************
void SecondsToTime(unsigned int dwSeconds, TTime* pTime)
{
	int year=BASETIME, month=1, day=1, hour=0, minute=0, second=0;
	unsigned int delta;

	while (dwSeconds > 0)
	{
		if ((year%4==0 && year%100!=0) || year%400==0)   //����
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
			if ((year%4==0 && year%100!=0) || year%400==0)   //����
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
// ��������: IsTimeEmpty
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
// ��    ��: sUesTcxF
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
// ��������: HexToStr
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
// ��    ��: sUesTcxF
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
// ��������: StrToHex
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
// ��    ��: sUesTcxF
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
// ��������: IsBcdCode
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
// ��    ��: sUesTcxF
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
// ��������: BcdToByte
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
// ��    ��: sUesTcxF
//*****************************************************************************
char BcdToByte(char bcd) 
{
	return ((bcd >> 4) & 0x0f) * 10 + (bcd & 0x0f);
}

//*****************************************************************************
// ��������: ByteToBcd
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
// ��    ��: sUesTcxF
//*****************************************************************************
char ByteToBcd(char b)
{
	return (b/10<<4) + b%10;
}

//*****************************************************************************
// ��������: BcdToDWORD
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
// ��    ��: sUesTcxF
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
// ��������: BcdToWORD
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
// ��    ��: sUesTcxF
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
// ��������: BcdToDDWORD
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
// ��    ��: sUesTcxF
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
// ��������: ByteToWord
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
// ��    ��: sUesTcxF
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
// ��������: ByteToDWORD
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
// ��    ��: sUesTcxF
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
// ��������: BcdToUint64
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
// ��    ��: sUesTcxF
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
// ��������: BcdToInt
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
// ��    ��: sUesTcxF
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
// ��������: WordToBcd
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
// ��    ��: sUesTcxF
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
// ��������: IntToBCD
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
// ��    ��: sUesTcxF
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
// ��������: DWORDToBCD
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
// ��    ��: sUesTcxF
//*****************************************************************************
void DWORDToBCD(unsigned int val, char* bcd, unsigned short len)
{
	unsigned int power;
	char bHigh, bLow;

	memset(bcd,0,len);
	if (len > 4)//��������ֽ�
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
// ��������: DWordToByte
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
// ��    ��: sUesTcxF
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
// ��������: DWordToByte
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
// ��    ��: sUesTcxF
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
// ��������: ByteToDWord
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
// ��    ��: sUesTcxF
//*****************************************************************************
unsigned int ByteToDWord(char* p)
{
	unsigned int dw = 0;
	memcpy(&dw, p, 4);
	return dw;
	//return *p + (DWORD )*(p + 1)*0x100 + (DWORD )*(p + 2)*0x10000 + (DWORD )*(p + 3)*0x1000000;
}

//*****************************************************************************
// ��������: Uint64ToBCD
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
// ��    ��: sUesTcxF
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
// ��������: CheckSum
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
// ��    ��: sUesTcxF
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


