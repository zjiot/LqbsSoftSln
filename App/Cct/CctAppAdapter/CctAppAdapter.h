//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : CctAppAdapter.h
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : CctAppҵ��ģ��������
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************
#ifndef CCTAPPADAPTER_H
#define CCTAPPADAPTER_H

#include <map>
#include "SocketIf.h"
#include "AppAdapter.h"
#include "CctMain.h"
using namespace std;

typedef enum
{
	ADAPTER_CCT_APP_FN0 = 0x0000,	//��½
	ADAPTER_CCT_APP_FN1,			//����
	ADAPTER_CCT_APP_FN2,			//���õ��������
	ADAPTER_CCT_APP_FN3,			//��ȡȫ�����һ��������
	ADAPTER_CCT_APP_FN4,			//��ȡһ�����һ��������
	ADAPTER_CCT_APP_FN5,			//�¼�
	ADAPTER_CCT_APP_FN6,			//CCO����
	ADAPTER_CCT_APP_FN7,			//Զ������
	ADAPTER_CCT_APP_FN8,			//���汾��

	//���FN...

	ADAPTER_CCT_APP_FN_MAX,

}ADAPTER_CCT_APP_FN_ENUM;

#pragma pack(1)
typedef struct
{
	unsigned char bAppName[13];
	TTime tTime;
}CCT_APP_INFO;
#pragma pack()  

typedef enum
{
	CCT_APP_ID_BASE = 0x0000,
	CCT_APP_ID_CCT,

	//���APP_ID

	CCT_APP_ID_MAX,

}IOT_APP_ID_ENUM;


#define POS_CMDFN_CODE			8
#define POS_645FRM_POS			12
#define POS_645FRM_LEN_POS		9	
#define POS_TMADDR_POS			3	

#define MAX_ADAPTER_FRM_LEN		2048
#define TERMINAL_ADDR_SIZE		6
#define MIN_ADAPTER_LEN			11

#define ADAPTER_FRM_LEN			2	//֡������ռ�ֽ���
#define ADAPTER_CMDOCDE_LEN		2	//�ն˵�ַ��ռ�ֽ���

class CCctAppAdapter;

typedef signed int (CCctAppAdapter::*DOCMDFN)(unsigned char *, unsigned char *);
typedef std::map<unsigned short, DOCMDFN>::iterator			ADAPTER_CDMFN_ITER;
typedef std::pair<unsigned short, DOCMDFN>					ADAPTER_CMDFN_PAIR;


extern CCctReader* g_pCctReader[READER_NUM];
extern CCT_APP_INFO g_AppVersion;

#pragma pack(1)
typedef struct
{
	unsigned char bAddr[6];		//����ַ
	unsigned char bPoroto;		//Э��汾97-07
	unsigned char bPort;		//����˿�1-31
	unsigned char bPhyase;		//��������
	unsigned short wCTPT;		//CTPT���
	unsigned char bValid;		//��������Ч�Ա�־
	unsigned char bVip;			//�ص��û����
	unsigned int  dwReverse;	//����
}CCT_MTR_PARA;

typedef struct
{
	unsigned char bCount;		//�������
	CCT_MTR_PARA tMtrPara[64];	//������
	
}CCT_SET_MTR_PARA;
#pragma pack()  

class CCctAppAdapter : public CAppAdapter
{
public:
	CCctAppAdapter();
	~CCctAppAdapter();


	bool Init();

	virtual bool RcvFrm(unsigned char *pBuf, unsigned int *pdwLen);
	virtual bool Send(unsigned char *pBuf, unsigned int dwlen);


	//ע�������ӿ�
	void AddCmdFn(unsigned short wCmdCode, DOCMDFN pFunc);

	virtual void DeleteCmd(unsigned short  wCmdCode);

	//�����
	signed int CctAppLogin(unsigned char *pInBuf, unsigned char *pOutBuf);
	signed int CctAppHeartBeat(ADAPTER_CCT_APP_FN_ENUM wCmdCode, 
								unsigned char *pRxBuf, unsigned short dwRxLen, 
								unsigned char *pTxBuf, unsigned short dwTxLen);
	signed int CctAppSetMtrPara(unsigned char *pInBuf, unsigned char *pOutBuf);
	signed int CctReadOneMtrData(unsigned char *pInBuf, unsigned char *pOutBuf);
	signed int CctReadAllMtrData(unsigned char *pInBuf, unsigned char *pOutBuf);
	signed int SetTime(unsigned char *pInBuf, unsigned char *pOutBuf);
	signed int CctGetCcoTop(unsigned char *pInBuf, unsigned char *pOutBuf);
	
	void       CctGetEvetMtrData();

	virtual signed int AdapterDoPackage(ADAPTER_CCT_APP_FN_ENUM wFn, 
												unsigned char *pRxBuf, unsigned short dwRxLen, 
												unsigned char *pTxBuf, unsigned short dwTxLen);
	
	virtual signed int DispatchPackage(unsigned char *pInBuf, unsigned char *pOutBuf);
	virtual bool MakeAdapterFrm(unsigned short wCmdCode, unsigned char *pInBuf,
										unsigned short wInBufLen, unsigned char *pOutBuf,
										unsigned short *pwOutBufLen);

	unsigned short GetCmdCode(unsigned char *pBuf);
	bool Get645FrmFromBuf(unsigned char *pInBuf, unsigned short wInBufLen, 
								unsigned char *pOutBuf, unsigned short *pwOutBufLen);
	bool GetAppData(unsigned char *pInBuf, unsigned char *pOutBuf);

	bool Login();
	bool HeartBeat();
	bool ConnectAdapter();
	bool IsAdapterConnected();
	bool GetTerminalAddr(unsigned char *pAddr);
	void CctAppSoftUpgrade();
	void CctAppGetVersion();
	void AdapterDoSaveMainNode(unsigned char *pBuf, unsigned int dwlen);
	signed int AdapterSendPackage2Svr(unsigned char *pBuf, unsigned short wLen);

	bool CheckTmAddr(unsigned char *pInBuf);


	
private:
	bool m_fAdapterConnected;
	
	CSocketIf *m_pIf;

	std::map<unsigned short, DOCMDFN> m_CmdFnMap;

	std::map<unsigned short, DOCMDFN> GetCmdFnMap() { return m_CmdFnMap;};

	unsigned char m_bTmAddr[TERMINAL_ADDR_SIZE];

};


void RegisterAppAdapter(IOT_APP_ID_ENUM wAppId);

#endif


