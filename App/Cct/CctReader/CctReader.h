//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : CctReader.h
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : PLC抄表父类
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************


#ifndef CCTREADER_H
#define CCTREADER_H
#pragma once


#include "CctMain.h"
#include "Comm.h"

//路由工作状态
#define ROUTER_STA_READ				0		//自动抄表模式
#define ROUTER_STA_IDLE				1		//空闲模式
#define ROUTER_STA_LEARN_ALL		2		//全面学习
#define ROUTER_STA_LEARN_LOCAL		3		//局部学习
#define ROUTER_STA_OPTIMIZE			4		//路由优化
#define ROUTER_STA_EXIT				5		//退出当前工作
#define ROUTER_STA_NO_ROUTER		6   	//模块拔出

#define ROUTER_LINK_GB				0x00	//376.2标准协议
#define ROUTER_LINK_RS485  	 		0x01	//485抄表
#define ROUTER_LINK_HISI		  	0x03	//海思方案
#define ROUTER_LINK_UNKNOW     		0xff	//未知类型


#define RT_OP_CLASSONE				1		//一类数据
#define RT_OP_CLASSTWO				2		//二类数据
#define	RT_OP_ALLCLASS				3		//所有数据类


#define POINT_NUM					1000	//测量点数量


#define	RDITEM_POSTFIX 				0		//后置
#define	RDITEM_PREFIX				1		//前置
#define	RDITEM_DATA_MAX				32		//附加数据的最大长度

//3762-AFN
#define CCT_AFN_3762_CON			0x00	//确认∕否认
#define CCT_AFN_3762_INIT			0x01	//初始化
#define CCT_AFN_3762_TRAN			0x02	//数据转发
#define CCT_AFN_3762_QRYDATA		0x03	//查询数据
#define CCT_AFN_3762_LNKTEST		0x04	//链路接口检测
#define CCT_AFN_3762_CTRL			0x05	//控制命令
#define CCT_AFN_3762_REP			0x06	//主动上报
#define CCT_AFN_3762_QRYRT			0x10	//路由查询
#define CCT_AFN_3762_SETRT		    0x11	//路由设置
#define CCT_AFN_3762_CTRLRT			0x12	//路由控制
#define CCT_AFN_3762_RTFWD 		    0x13	//路由数据转发
#define CCT_AFN_3762_RTRD 		    0x14	//路由数据抄读
#define CCT_AFN_3762_TRSFILE   		0x15    //文件传输


//南网数据处理接口///////////////////////////////////////////////////////////////////////////////////
#define CCT_AFN_CSG_READ_PARA		0x03	//南网读参数
#define CCT_AFN_CSG_TASK_REP		0x05	//南网上报信息

#define CCT_DI_CSG_PARA_DATA		0xE8000303	//上报参数


#define CCT_DI_CSG_TASK_DATA		0xE8050501	//南网上报信息
#define CCT_DI_CSG_ERC_DATA			0xE8050502	//南网上报信息
#define CCT_DI_CSG_POWERFAIL_DATA	0xE8050506	//南网上报信息
//南网数据处理接口///////////////////////////////////////////////////////////////////////////////////

//3762-FN
#define FN1							1
#define FN2							2
#define FN3							3
#define FN4							4
#define FN5							5
#define FN6							6
#define FN7							7
#define FN8							8
#define FN9							9
#define FN10						10
#define FN11						11

#define F21							21
#define FN228						228


//帧控制域C
#define FRM_C_PRM_1		            0x01	//DIR,启动
#define FRM_C_PRM_0			        0x00	//DIR,从动

#pragma pack(1)  

//方向，启动站，集中式路由
typedef struct
{
	unsigned char bDir :1;
	unsigned char bPrm :1;
	unsigned char bCn :6;
	
}TCtrl;


typedef struct
{
	unsigned char bLogicPort;		//逻辑端口号,指的是通信协议上给每个通道规定的端口号,而不是物理端口号
	unsigned char bPhyPort;			//物理端口号
	TCommPara tCommPara;			//串口参数
	
	unsigned char bMtrPro;			//全局电表协议协议版本,CCT_MTRPRO_97/CCT_MTRPRO_07
									//本参数只用到个别全局命令中,比如广播抄表,
									//对于某个电表的通信协议,通过GetPnMtrPro()来获取

	unsigned short wHourRdTimes;	//小时选抄每一轮的尝试次数
	unsigned short wMinRdTimes;		//分钟选抄每一轮的尝试次数

	unsigned int dwHourRerdInterv;	//小时补抄间隔,0表示只尝试1轮不进行补抄,单位分钟
	unsigned char  bHourDelay;		//小时抄读延迟时间,单位分钟,主用用来避免终端时间比电表快时,
									//抄读电表冻结的时标不正确,终端延迟一段时间再去抄读
	unsigned char  bDayFrzDelay;	//日冻结抄读延迟时间,单位分钟,主用用来避免终端时间比电表快

	bool fUseLoopBuf;				//是否使用循环缓冲区
	unsigned short wLoopBufSize;	//循环缓冲区的大小
	unsigned char bMainAddr[6];		//主节点地址

	unsigned char  bReRdTmFlg[36];	//补抄的时段标志,36=12*24/8
									//00:00~24:00,每一个比特表示5分钟,置1表示该时间段内要进行补抄,否则不做

	//选抄控制
	unsigned char  bSelRdTmFlg[36];	//选抄的时段标志,36=12*24/8
									//00:00~24:00,每一个比特表示5分钟,置1表示该时间段内要进行补抄,否则不做
	
	unsigned short wInfoPlcPara;
	unsigned short wInfoPlcClrRt;
	unsigned short wInfoPlcStopRd;
	unsigned short wInfoPlcResumeRd;
	unsigned short wInfoPlcRdAllRtInfo;//读取所有节点的中继路由信息
	unsigned short wInfoUpdateRT;	//载波路由器升级
	unsigned short wInfoChannelChange;
}TCctRdrPara;//自动抄表器参数

typedef struct
{
	TCctRdrPara RdrPara;			//自动抄表器参数
	unsigned char bLink;			//链路类型,用来区分不同厂家
	bool fParTask;					//是否启用并行抄表任务
    unsigned short wFrmLenBytes;	//帧长度中的字节个数,1表示1个字节,2表示2个字节
	unsigned char bRunMode;         //模块运行模式1、2、3代

}TCctPara;							//标准路由参数


typedef struct 
{
	unsigned int  dwID;				//数据项ID:对于97协议用到2个字节,对于07协议用到4个字节
	unsigned short wPn;  			//测量点号
	bool fByAcqUnit;				//是否通过低压采集器(acquisition unit)抄读
	unsigned char bMtrPro;			//电表通信协议:CCT_MTRPROTO_97/CCT_MTRPROTO_07
	unsigned char bDataLen;			//附加数据长度,不包括ID
	unsigned char bDataFix;			//附加数据放在ID的前/后(RDITEM_PREFIX/RDITEM_POSTFIX),
	unsigned char bData[RDITEM_DATA_MAX]; //读数据项的附加数据,不包括ID,写数据项不要这个来传递数据
}TCctRwItem; 						//抄读数据项的控制结构

typedef struct
{
	unsigned short wNodeSeq;
	unsigned char bNodeNum;
}TTopPara;

#pragma pack()  


class CCctReader
{
public:
	CCctReader(void);
	virtual ~CCctReader(void);

	bool Init(TCctPara* pCctPara);
	void SetState(unsigned char bState) { m_bState = bState; };//切换当前工作状态
	void Run(void *pvArg);

	
	void DoCctUpdate();
	void SetCctPara(TCctPara *pCctPara);
	virtual void InitRun() {};
	
	virtual bool StopRouter();	
	virtual bool ResumeRouter();
	
	virtual void LockReader() { };
	virtual void UnLockReader() { };	
	virtual bool DefHanleFrm(){return false;};
	
	//被动(通道型)路由抄表 XC、RSC、HISI AFN=1301 新增被动路由重写此接口
	virtual void DoCctReader(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize);	
	virtual void FnToDt(unsigned short wFn, unsigned char* pbDt);
	//主动路由抄表 TC、ES、LH AFN=1401	新增主动路由重写此接口
	virtual signed int DoAutoCctReader(){return 0;};	
	virtual signed int DoOptimize() { SetState(ROUTER_STA_READ); return 0; };
	virtual signed int DoIdle() { return 0; };
	virtual signed int DoLearnAll() { SetState(ROUTER_STA_READ); return 0; };
	virtual signed int DoLearnLocal() { SetState(ROUTER_STA_READ); return 0; };
	virtual signed int UpdateCctReader() { return 0; };
	virtual signed int CctReadItemData(unsigned char *pInBuf, unsigned int dwInBufLen, 
												unsigned char *pOutBuf, unsigned int* pOutBufLen) { return 0; };

	virtual signed int DoPointRead();
	virtual signed int DoMinPointRead();
	virtual signed int DoHourPointRead();

	virtual void DoCctRcvFrm();
	
	virtual bool RcvFrm(unsigned char *pRx, unsigned int *pRxLen);
	virtual signed int Send(unsigned char *pTx, unsigned int dwTxLen);
	virtual signed int Make3762ReadFrm(unsigned char* pbTxBuf, unsigned int *pdwLen, 
												unsigned char bAfn, unsigned short wFn, 
												unsigned char* pbData, unsigned short wDataLen);
	
	virtual unsigned short MakeAppDataField(unsigned char bAfn, unsigned short wFn, unsigned char* pbData, 
													unsigned short wLen, unsigned char* pbDataField);

	bool IsNeedSeq(unsigned char bAfn);
	void SetAddrField(unsigned char *pbTxBuf, unsigned char *pbAddr);
	void GetAcqAddr(unsigned char *pMtrAddr, unsigned char *pAcqAddr);


	bool CtrlRoute(unsigned short wFn);
	bool MakeCtrlFrm(unsigned char *pBuf, unsigned int *pdwLen, unsigned char bAFN, unsigned short wFn);
	bool Make3762Frm(unsigned char *pBuf, unsigned int *pdwLen, unsigned char bAFN, 
								unsigned short wFn, unsigned char *pInBuf, unsigned int dwLen);

	virtual signed int CctReaderMsgProc(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize);

	virtual signed int Get645FrmSizeFrom3762Frm(unsigned char *p3762Frm, unsigned int *p645FrmSize);

	void GetCctReaderCcoTop();
	void DoRouterQueryFrm(unsigned char *pBuf, unsigned short wLen);
	void DoAutoRepFrm(unsigned char *pBuf, unsigned short wLen);
	bool IsAddrRegion(unsigned char *pBuf, unsigned short wLen);
	unsigned char GetAfn(unsigned char *pBuf, unsigned short wLen);
	void DoDispatchFrm(unsigned char *pBuf, unsigned short wLen);
	unsigned short DtToFn(unsigned char* pbDt);
	signed int DoDispatchParaFrm(unsigned char *pBuf, unsigned short wLen);

	unsigned char* GetMainAddr();
	signed int ReadMainAddr();
	void CctReaderTaskKeepAlive();
	void DoRouterConFrm(unsigned char *pBuf, unsigned short wLen);
	void CheckUartPulse(unsigned char *pBuf);
	void CctReaderTaskUartPulse();
	void CctConfirm();
	void CctRecvTaskKeepAlive();


//南网数据处理接口///////////////////////////////////////////////////////////////////////////////////
	virtual void DoCsgCctRcvFrm();
	virtual signed int Make3762CsgReadFrm(unsigned char* pbTxBuf, unsigned int *pdwLen, 
												unsigned char bAfn, unsigned int dwFn, 
												unsigned char* pbData, unsigned short wDataLen);
	
	bool IsCsgAddrRegion(unsigned char *pBuf, unsigned short wLen);
	unsigned char GetCsgAfn(unsigned char *pBuf, unsigned short wLen);
	

	signed int DoCsgDispatchTaskRepFrm(unsigned char *pInBuf, unsigned short wInLen,
																unsigned char *pOutBuf, unsigned int *pdwOutLen);
	
	signed int  GetCsgDataFromTaskRepFrm(unsigned int dwDI, unsigned char *pInBuf, unsigned short wInLen,
											unsigned char *pOutBuf, unsigned int *pdwOutLen);

	signed int GetCsgEvtSendData(unsigned char *pInBuf, unsigned short wInLen,
												unsigned char *pOutBuf, unsigned int *pdwOutLen);
	
	unsigned int GetCsgDI(unsigned char *pBuf, unsigned short wLen);
	

	signed int ReadCsgMainAddr();
	signed int MakeCsgQueryMainNodeFrm(unsigned char* pbTxBuf, unsigned int *pdwLen,
										unsigned char bAfn, unsigned int dwFn,
										unsigned char* pbData, unsigned short wDataLen);
	signed int GetCsgMainAddr(unsigned char *pInBuf, unsigned short wInLen);
	signed int  DoDispatchCsgParaFrm(unsigned char *pBuf, unsigned short wLen);
	void CctCsgConfirm();
//南网接口///////////////////////////////////////////////////////////////////////////////////

protected:
	
	TCctPara *m_pCctPara;	
	char m_bLinkType;				//物理链路的类型(路由类型)
	char m_fPlcMoudleChange;		//PLC模块变更
	char m_bState;
	bool m_fStopRdMtr;
	bool m_fStopRouter;
	bool m_fInReadPeriod;
	bool m_fSchMtr;
	bool m_fInInterVRead;
	bool m_fDirectOpera;
	unsigned short m_wRdTurn;		//0首轮抄表 1为第1次补抄
	signed int  m_dwStudyClick;		//抄完一轮后路由的学习时间
	signed int  m_dwDirReadClick;	//上一次操作路由器时标，如点抄、停止、启动路由等操作			 

	CComm *m_pComm;

public:
	unsigned char *m_pInBuf;
	unsigned int m_dwInBufLen;
	unsigned char *m_pOutBuf;
	unsigned int m_dwOutBufLen;

	unsigned char *m_pTx;
	unsigned int m_TxLen;
	unsigned char *m_pRx;
	unsigned int m_RxLen;

	unsigned char m_bRxSN;			//发送报文序列号
	unsigned char m_bTxSN;			//接受报文序列号


	unsigned short m_wTaskNo;		//南网任务号
	unsigned short m_wTaskTimeOut;	//南网超时时长

	unsigned char m_bMainAddr[6];
	bool m_fIsMainNodeAddrOk;

	unsigned int m_dwUartPulseClick;
	
};

void AddDebugPowerOffData2Db(char bFlag);

#endif


