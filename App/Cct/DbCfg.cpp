//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : DbCfg.cpp
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
#include "DbCfg.h"
#include "OnTimer.h"


TCctRdCtrl pCctRdCtrlTbl[] =
{
	{0x05060101,  TIME_INTU_DAY,	1, 		0,		{0,  0,  0,  0,  1,  0},	{1,PNCHAR_ALL},			    		2, {1, M07 | PH13 | DYMTR}},	//上次日冻结时间       日冻结抄电表冻结
	{0x05060201,  TIME_INTU_DAY,	1, 		0,		{0,  0,  0,  0,  1,  0},	{1,PNCHAR_ALL},		   				2, {1, M07 | PH13 | DYMTR}},	//日冻结, 正向有功电能示值(总、费率1~M)	

};

//*****************************************************************************
// 函数名称: GetIDCount
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
unsigned short GetIDCount()
{
	return sizeof(pCctRdCtrlTbl) / sizeof(TCctRdCtrl);
}

//*****************************************************************************
// 函数名称: GetCctIDIntVTable
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
unsigned char GetCctIDIntVTable(unsigned char dwID)
{
	GetIDCount();


	for (unsigned short i = 0; i < GetIDCount(); i++)
	{
		if (pCctRdCtrlTbl[i].dwID == dwID)
		{
			return pCctRdCtrlTbl[i].bIntervU;
		}
	}

	//默认返回小时抄读间隔
	return TIME_INTU_HOUR;
}

