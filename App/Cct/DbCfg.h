//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : DbCfg.h
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 接口类
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************
#include "arch.h"

#ifndef DBCFG_H
#define DBCFG_H



#define PNCHAR_MAX	16			//最大支持的测量点特征字个数
#define PNCHAR_ALL	0			//所有测量点特征字,放在测量点特征字个数字节,用来表示这种特殊情况
#define PNCHAR_VIP	0xff		//重点户测量点特征字,放在测量点特征字个数字节,用来表示这种特殊情况

#define PH1			(1<<0)		//允许单相表抄读
#define PH3			(1<<1)		//允许3相表抄读
#define M9712		(1<<2)		//允许97表非块抄读2费率
#define M9711		(1<<3)		//允许97表非块抄读4费率
#define M97			(1<<4)		//允许97表抄读
#define M07			(1<<5)		//允许07表抄读

#define DYCUR		(1<<6)		//DAY FROZEN CURRENT    在日冻结配置成抄电表当前数据时抄读
#define DYMTR		(1<<7)		//DAY FROZEN METER FRZ  在日冻结配置成抄电表冻结时抄读

#define DFCUR		(1<<8)		//DAYFLG FROZEN CURRENT 在抄表日冻结配置成抄电表当前数据时抄读
#define DFMTR		(1<<9)		//DAYFLG FROZEN METER   在抄表日冻结配置成抄电表冻结时抄读

#define MHCUR		(1<<10) 	//MONTH FROZEN CURRENT     在月冻结配置成抄电表当前数据时抄读
#define MHMTR		(1<<11)		//MONTH FROZEN METER FRZ   在月冻结配置成抄电表冻结数据时抄读
#define MHLST		(1<<12)     //MONTH FROZEN METER LAST  在月冻结配置成抄电表上一结算日数据时抄读

#define PFCUR		(1<<13)		//PROFILE FROZEN CURRENT 在曲线配置成抄电表当前数据时抄读
#define PFMTR		(1<<14)		//PROFILE FROZEN METER   在曲线配置成抄电表冻结时抄读

#define DEMDCUR	    (1<<15)		//DEMAND DAY FROZEN CURRENT   需量在日冻结配置成抄电表当前数据时抄读
#define DEMDMTR	    (1<<16)		//DEMAND DAY FROZEN METER FRZ 需量在日冻结配置成抄电表冻结时抄读

#define DEMMCUR	    (1<<17)		//DEMAND MONTH FROZEN CURRENT    需量在月冻结配置成抄电表当前数据时抄读
#define DEMMMTR	    (1<<18)		//DEMAND MONTH FROZEN METER FRZ  需量在月冻结配置成抄电表冻结时抄读
#define DEMMLST	    (1<<19)		//DEMAND MONTH FROZEN LAST       需量在月冻结配置成抄上一结算日数据时抄读


#define DEMDFCUR	(1<<20)		//DEMAND DAYFLG FROZEN CURRENT    需量在抄表日月冻结配置成抄电表当前数据时抄读
#define DEMDFMTR	(1<<21)		//DEMAND DAYFLG FROZEN METER FRZ  需量在抄表日月冻结配置成抄电表冻结时抄读


#define PH13		(PH1|PH3)	//允许单相/3相表抄读
#define M0797		(M97|M07)	//允许07/97表抄读
#define M97S		(M9711|M9712)//允许97表抄非块读
#define ALRID       (1<<22)     //告警ID


typedef struct
{
	unsigned int   dwID;				//内部ID
	unsigned char  bIntervU;			//抄表间隔单位
	unsigned char  bIntervV;			//抄表间隔值
	unsigned short wInterID;			//抄表开始时间ID(该项为0，则取tmStartTime为开始时间)
	TTime 		   tmStartTime;			//开始时间
	unsigned char  bPnChar[PNCHAR_MAX];	//要执行此任务的测量点特征字
		                                //对于集抄,分别配置要多个不同类型的电表特征字,第一个字节表示特征字的个数,后面才是一个个电表特征字
										//某些地方只需要配置一个特征字,直接配置到bPnChar[0]即可,不用特征字个数了
										//PNCHAR_ALL 所有测量点特征字,放在测量点特征字个数字节,用来表示这种特殊情况
										//PNCHAR_VIP 重点户测量点特征字,放在测量点特征字个数字节,用来表示这种特殊情况
	unsigned char  bClass;				//数据类型
	unsigned int   dwCtlFlg[PNCHAR_MAX];//控制标志位,对于国标来说dwCtlFlg[]的每个元素与bPnChar[]一一对应，
										//分别控制每个FN在什么情况下进行抄读
}TCctRdCtrl;							//集抄抄读控制结构

extern TCctRdCtrl pCctRdCtrlTbl[];

unsigned short GetIDCount();
unsigned char GetCctIDIntVTable(unsigned char dwID);

	
#endif


