//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : CctCfg.h
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


#ifndef CCTCFG_H
#define CCTCFG_H
#pragma once

#include "CctReader.h"

void GetDbPara(TCctDbPara *pDbpara);
void LoadCctPlcCom(TCommPara *pCommPara);
void LoadSocketPara(TSocketPara * pSocketPara);
void LoadSvrSocketPara(TSocketPara *pSocketPara);
void LoadCctPara(char bLink, TCctPara *pCctPara);
bool IsBufferAllZero(unsigned char *pbBuf, unsigned short wLen);
bool IsBufferAllEE(unsigned char *pbBuf, unsigned short wLen);
bool IsBufferAllVal(unsigned char *pbBuf, unsigned short wLen, unsigned char bVal);


bool IsInReadPeriod();
const char* GetPlcPnMask();
signed int QueryItemReadStatus();
signed short SearchPnFromMask(const char *pbPnMask, unsigned short wPn);
signed short SearchDoUnReadID(short wPn, char bClass, char bLinkType, TCctRwItem *pRwIte);

unsigned char *GetMtrAddr(unsigned char *pFrmData);
unsigned char GetMtrProtocol(unsigned char * pAddr);
unsigned int GetDataID(unsigned char *pFrmData, unsigned char bMtrProtocol);
unsigned char GetDataLen(unsigned char *pFrmData, unsigned char bMtrProtocol);

unsigned int DaysFrom2000(const TTime& time);
unsigned int TimeToSeconds(const TTime& time);
void SecondsToTime(unsigned int dwSeconds, TTime* pTime);

bool IsTimeEmpty(const TTime& time);
void HexToStr(char *pbDest, char *pbSrc, unsigned int nLen);
void StrToHex(char *pbDest, char *pbSrc, unsigned int nLen);


bool IsBcdCode(char *p, unsigned short num);
char BcdToByte(char bcd);
char ByteToBcd(char b);
int BcdToInt(char* p, unsigned short len);
void IntToBCD(int val, char* bcd, unsigned short len);
void DWORDToBCD(unsigned int val, char* bcd, unsigned short len);
void DWordToByte(unsigned int val, char* byte, unsigned short len);
void Uint64ToBCD(unsigned long long val, char* bcd, unsigned short len);
unsigned short DWordToByte(unsigned int dw, char* p);
unsigned int ByteToDWord(char* p);
unsigned int BcdToDWORD(char* p, unsigned short len);
unsigned int ByteToDWORD(char* pbBuf, unsigned short wLen);
unsigned short BcdToWORD(char * p) ;
unsigned short ByteToWord(char* pbBuf, unsigned short wLen);
unsigned long long BcdToUint64(char* p, unsigned short len);
unsigned long long BcdToDDWORD(char* p, unsigned short len);
unsigned short WordToBcd(unsigned short num);
unsigned char CheckSum(unsigned char* p, unsigned short wLen);




#endif


