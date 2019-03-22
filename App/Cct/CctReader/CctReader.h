//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : CctReader.h
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : PLC������
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************


#ifndef CCTREADER_H
#define CCTREADER_H
#pragma once


#include "CctMain.h"
#include "Comm.h"

//·�ɹ���״̬
#define ROUTER_STA_READ				0		//�Զ�����ģʽ
#define ROUTER_STA_IDLE				1		//����ģʽ
#define ROUTER_STA_LEARN_ALL		2		//ȫ��ѧϰ
#define ROUTER_STA_LEARN_LOCAL		3		//�ֲ�ѧϰ
#define ROUTER_STA_OPTIMIZE			4		//·���Ż�
#define ROUTER_STA_EXIT				5		//�˳���ǰ����
#define ROUTER_STA_NO_ROUTER		6   	//ģ��γ�

#define ROUTER_LINK_GB				0x00	//376.2��׼Э��
#define ROUTER_LINK_RS485  	 		0x01	//485����
#define ROUTER_LINK_HISI		  	0x03	//��˼����
#define ROUTER_LINK_UNKNOW     		0xff	//δ֪����


#define RT_OP_CLASSONE				1		//һ������
#define RT_OP_CLASSTWO				2		//��������
#define	RT_OP_ALLCLASS				3		//����������


#define POINT_NUM					1000	//����������


#define	RDITEM_POSTFIX 				0		//����
#define	RDITEM_PREFIX				1		//ǰ��
#define	RDITEM_DATA_MAX				32		//�������ݵ���󳤶�

//3762-AFN
#define CCT_AFN_3762_CON			0x00	//ȷ�ϨM����
#define CCT_AFN_3762_INIT			0x01	//��ʼ��
#define CCT_AFN_3762_TRAN			0x02	//����ת��
#define CCT_AFN_3762_QRYDATA		0x03	//��ѯ����
#define CCT_AFN_3762_LNKTEST		0x04	//��·�ӿڼ��
#define CCT_AFN_3762_CTRL			0x05	//��������
#define CCT_AFN_3762_REP			0x06	//�����ϱ�
#define CCT_AFN_3762_QRYRT			0x10	//·�ɲ�ѯ
#define CCT_AFN_3762_SETRT		    0x11	//·������
#define CCT_AFN_3762_CTRLRT			0x12	//·�ɿ���
#define CCT_AFN_3762_RTFWD 		    0x13	//·������ת��
#define CCT_AFN_3762_RTRD 		    0x14	//·�����ݳ���
#define CCT_AFN_3762_TRSFILE   		0x15    //�ļ�����


//�������ݴ���ӿ�///////////////////////////////////////////////////////////////////////////////////
#define CCT_AFN_CSG_READ_PARA		0x03	//����������
#define CCT_AFN_CSG_TASK_REP		0x05	//�����ϱ���Ϣ

#define CCT_DI_CSG_PARA_DATA		0xE8000303	//�ϱ�����


#define CCT_DI_CSG_TASK_DATA		0xE8050501	//�����ϱ���Ϣ
#define CCT_DI_CSG_ERC_DATA			0xE8050502	//�����ϱ���Ϣ
#define CCT_DI_CSG_POWERFAIL_DATA	0xE8050506	//�����ϱ���Ϣ
//�������ݴ���ӿ�///////////////////////////////////////////////////////////////////////////////////

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


//֡������C
#define FRM_C_PRM_1		            0x01	//DIR,����
#define FRM_C_PRM_0			        0x00	//DIR,�Ӷ�

#pragma pack(1)  

//��������վ������ʽ·��
typedef struct
{
	unsigned char bDir :1;
	unsigned char bPrm :1;
	unsigned char bCn :6;
	
}TCtrl;


typedef struct
{
	unsigned char bLogicPort;		//�߼��˿ں�,ָ����ͨ��Э���ϸ�ÿ��ͨ���涨�Ķ˿ں�,����������˿ں�
	unsigned char bPhyPort;			//����˿ں�
	TCommPara tCommPara;			//���ڲ���
	
	unsigned char bMtrPro;			//ȫ�ֵ��Э��Э��汾,CCT_MTRPRO_97/CCT_MTRPRO_07
									//������ֻ�õ�����ȫ��������,����㲥����,
									//����ĳ������ͨ��Э��,ͨ��GetPnMtrPro()����ȡ

	unsigned short wHourRdTimes;	//Сʱѡ��ÿһ�ֵĳ��Դ���
	unsigned short wMinRdTimes;		//����ѡ��ÿһ�ֵĳ��Դ���

	unsigned int dwHourRerdInterv;	//Сʱ�������,0��ʾֻ����1�ֲ����в���,��λ����
	unsigned char  bHourDelay;		//Сʱ�����ӳ�ʱ��,��λ����,�������������ն�ʱ��ȵ���ʱ,
									//����������ʱ�겻��ȷ,�ն��ӳ�һ��ʱ����ȥ����
	unsigned char  bDayFrzDelay;	//�ն��᳭���ӳ�ʱ��,��λ����,�������������ն�ʱ��ȵ���

	bool fUseLoopBuf;				//�Ƿ�ʹ��ѭ��������
	unsigned short wLoopBufSize;	//ѭ���������Ĵ�С
	unsigned char bMainAddr[6];		//���ڵ��ַ

	unsigned char  bReRdTmFlg[36];	//������ʱ�α�־,36=12*24/8
									//00:00~24:00,ÿһ�����ر�ʾ5����,��1��ʾ��ʱ�����Ҫ���в���,������

	//ѡ������
	unsigned char  bSelRdTmFlg[36];	//ѡ����ʱ�α�־,36=12*24/8
									//00:00~24:00,ÿһ�����ر�ʾ5����,��1��ʾ��ʱ�����Ҫ���в���,������
	
	unsigned short wInfoPlcPara;
	unsigned short wInfoPlcClrRt;
	unsigned short wInfoPlcStopRd;
	unsigned short wInfoPlcResumeRd;
	unsigned short wInfoPlcRdAllRtInfo;//��ȡ���нڵ���м�·����Ϣ
	unsigned short wInfoUpdateRT;	//�ز�·��������
	unsigned short wInfoChannelChange;
}TCctRdrPara;//�Զ�����������

typedef struct
{
	TCctRdrPara RdrPara;			//�Զ�����������
	unsigned char bLink;			//��·����,�������ֲ�ͬ����
	bool fParTask;					//�Ƿ����ò��г�������
    unsigned short wFrmLenBytes;	//֡�����е��ֽڸ���,1��ʾ1���ֽ�,2��ʾ2���ֽ�
	unsigned char bRunMode;         //ģ������ģʽ1��2��3��

}TCctPara;							//��׼·�ɲ���


typedef struct 
{
	unsigned int  dwID;				//������ID:����97Э���õ�2���ֽ�,����07Э���õ�4���ֽ�
	unsigned short wPn;  			//�������
	bool fByAcqUnit;				//�Ƿ�ͨ����ѹ�ɼ���(acquisition unit)����
	unsigned char bMtrPro;			//���ͨ��Э��:CCT_MTRPROTO_97/CCT_MTRPROTO_07
	unsigned char bDataLen;			//�������ݳ���,������ID
	unsigned char bDataFix;			//�������ݷ���ID��ǰ/��(RDITEM_PREFIX/RDITEM_POSTFIX),
	unsigned char bData[RDITEM_DATA_MAX]; //��������ĸ�������,������ID,д�����Ҫ�������������
}TCctRwItem; 						//����������Ŀ��ƽṹ

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
	void SetState(unsigned char bState) { m_bState = bState; };//�л���ǰ����״̬
	void Run(void *pvArg);

	
	void DoCctUpdate();
	void SetCctPara(TCctPara *pCctPara);
	virtual void InitRun() {};
	
	virtual bool StopRouter();	
	virtual bool ResumeRouter();
	
	virtual void LockReader() { };
	virtual void UnLockReader() { };	
	virtual bool DefHanleFrm(){return false;};
	
	//����(ͨ����)·�ɳ��� XC��RSC��HISI AFN=1301 ��������·����д�˽ӿ�
	virtual void DoCctReader(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize);	
	virtual void FnToDt(unsigned short wFn, unsigned char* pbDt);
	//����·�ɳ��� TC��ES��LH AFN=1401	��������·����д�˽ӿ�
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


//�������ݴ���ӿ�///////////////////////////////////////////////////////////////////////////////////
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
//�����ӿ�///////////////////////////////////////////////////////////////////////////////////

protected:
	
	TCctPara *m_pCctPara;	
	char m_bLinkType;				//������·������(·������)
	char m_fPlcMoudleChange;		//PLCģ����
	char m_bState;
	bool m_fStopRdMtr;
	bool m_fStopRouter;
	bool m_fInReadPeriod;
	bool m_fSchMtr;
	bool m_fInInterVRead;
	bool m_fDirectOpera;
	unsigned short m_wRdTurn;		//0���ֳ��� 1Ϊ��1�β���
	signed int  m_dwStudyClick;		//����һ�ֺ�·�ɵ�ѧϰʱ��
	signed int  m_dwDirReadClick;	//��һ�β���·����ʱ�꣬��㳭��ֹͣ������·�ɵȲ���			 

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

	unsigned char m_bRxSN;			//���ͱ������к�
	unsigned char m_bTxSN;			//���ܱ������к�


	unsigned short m_wTaskNo;		//���������
	unsigned short m_wTaskTimeOut;	//������ʱʱ��

	unsigned char m_bMainAddr[6];
	bool m_fIsMainNodeAddrOk;

	unsigned int m_dwUartPulseClick;
	
};

void AddDebugPowerOffData2Db(char bFlag);

#endif


