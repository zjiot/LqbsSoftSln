//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : DbCfg.cpp
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
#include "DbCfg.h"
#include "OnTimer.h"


TCctRdCtrl pCctRdCtrlTbl[] =
{
	{0x05060101,  TIME_INTU_DAY,	1, 		0,		{0,  0,  0,  0,  1,  0},	{1,PNCHAR_ALL},			    		2, {1, M07 | PH13 | DYMTR}},	//�ϴ��ն���ʱ��       �ն��᳭�����
	{0x05060201,  TIME_INTU_DAY,	1, 		0,		{0,  0,  0,  0,  1,  0},	{1,PNCHAR_ALL},		   				2, {1, M07 | PH13 | DYMTR}},	//�ն���, �����й�����ʾֵ(�ܡ�����1~M)	

};

//*****************************************************************************
// ��������: GetIDCount
// ��������: 
//
// ����˵��:
// dwQueId  -  in��dwMsgId  -  in��pBuf  -  in��dwSize  -  in
// 
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
unsigned short GetIDCount()
{
	return sizeof(pCctRdCtrlTbl) / sizeof(TCctRdCtrl);
}

//*****************************************************************************
// ��������: GetCctIDIntVTable
// ��������: 
//
// ����˵��:
// dwQueId  -  in��dwMsgId  -  in��pBuf  -  in��dwSize  -  in
// 
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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

	//Ĭ�Ϸ���Сʱ�������
	return TIME_INTU_HOUR;
}

