//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : DbCfg.h
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : �ӿ���
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************
#include "arch.h"

#ifndef DBCFG_H
#define DBCFG_H



#define PNCHAR_MAX	16			//���֧�ֵĲ����������ָ���
#define PNCHAR_ALL	0			//���в�����������,���ڲ����������ָ����ֽ�,������ʾ�����������
#define PNCHAR_VIP	0xff		//�ص㻧������������,���ڲ����������ָ����ֽ�,������ʾ�����������

#define PH1			(1<<0)		//���������
#define PH3			(1<<1)		//����3�����
#define M9712		(1<<2)		//����97��ǿ鳭��2����
#define M9711		(1<<3)		//����97��ǿ鳭��4����
#define M97			(1<<4)		//����97����
#define M07			(1<<5)		//����07����

#define DYCUR		(1<<6)		//DAY FROZEN CURRENT    ���ն������óɳ����ǰ����ʱ����
#define DYMTR		(1<<7)		//DAY FROZEN METER FRZ  ���ն������óɳ������ʱ����

#define DFCUR		(1<<8)		//DAYFLG FROZEN CURRENT �ڳ����ն������óɳ����ǰ����ʱ����
#define DFMTR		(1<<9)		//DAYFLG FROZEN METER   �ڳ����ն������óɳ������ʱ����

#define MHCUR		(1<<10) 	//MONTH FROZEN CURRENT     ���¶������óɳ����ǰ����ʱ����
#define MHMTR		(1<<11)		//MONTH FROZEN METER FRZ   ���¶������óɳ����������ʱ����
#define MHLST		(1<<12)     //MONTH FROZEN METER LAST  ���¶������óɳ������һ����������ʱ����

#define PFCUR		(1<<13)		//PROFILE FROZEN CURRENT ���������óɳ����ǰ����ʱ����
#define PFMTR		(1<<14)		//PROFILE FROZEN METER   ���������óɳ������ʱ����

#define DEMDCUR	    (1<<15)		//DEMAND DAY FROZEN CURRENT   �������ն������óɳ����ǰ����ʱ����
#define DEMDMTR	    (1<<16)		//DEMAND DAY FROZEN METER FRZ �������ն������óɳ������ʱ����

#define DEMMCUR	    (1<<17)		//DEMAND MONTH FROZEN CURRENT    �������¶������óɳ����ǰ����ʱ����
#define DEMMMTR	    (1<<18)		//DEMAND MONTH FROZEN METER FRZ  �������¶������óɳ������ʱ����
#define DEMMLST	    (1<<19)		//DEMAND MONTH FROZEN LAST       �������¶������óɳ���һ����������ʱ����


#define DEMDFCUR	(1<<20)		//DEMAND DAYFLG FROZEN CURRENT    �����ڳ������¶������óɳ����ǰ����ʱ����
#define DEMDFMTR	(1<<21)		//DEMAND DAYFLG FROZEN METER FRZ  �����ڳ������¶������óɳ������ʱ����


#define PH13		(PH1|PH3)	//������/3�����
#define M0797		(M97|M07)	//����07/97����
#define M97S		(M9711|M9712)//����97���ǿ��
#define ALRID       (1<<22)     //�澯ID


typedef struct
{
	unsigned int   dwID;				//�ڲ�ID
	unsigned char  bIntervU;			//��������λ
	unsigned char  bIntervV;			//������ֵ
	unsigned short wInterID;			//����ʼʱ��ID(����Ϊ0����ȡtmStartTimeΪ��ʼʱ��)
	TTime 		   tmStartTime;			//��ʼʱ��
	unsigned char  bPnChar[PNCHAR_MAX];	//Ҫִ�д�����Ĳ�����������
		                                //���ڼ���,�ֱ�����Ҫ�����ͬ���͵ĵ��������,��һ���ֽڱ�ʾ�����ֵĸ���,�������һ�������������
										//ĳЩ�ط�ֻ��Ҫ����һ��������,ֱ�����õ�bPnChar[0]����,���������ָ�����
										//PNCHAR_ALL ���в�����������,���ڲ����������ָ����ֽ�,������ʾ�����������
										//PNCHAR_VIP �ص㻧������������,���ڲ����������ָ����ֽ�,������ʾ�����������
	unsigned char  bClass;				//��������
	unsigned int   dwCtlFlg[PNCHAR_MAX];//���Ʊ�־λ,���ڹ�����˵dwCtlFlg[]��ÿ��Ԫ����bPnChar[]һһ��Ӧ��
										//�ֱ����ÿ��FN��ʲô����½��г���
}TCctRdCtrl;							//�����������ƽṹ

extern TCctRdCtrl pCctRdCtrlTbl[];

unsigned short GetIDCount();
unsigned char GetCctIDIntVTable(unsigned char dwID);

	
#endif


