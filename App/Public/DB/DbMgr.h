//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : DbMgr.h
//  �� �� ��   : V1.0
//  ��    ��   : tjl
//  ��������   : 2018-10-31
//  ��������   : 
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-31
//    ��    �� : sUesTcxF
//    �޸����� : 
//
//*****************************************************************************

#ifndef DBMGR_H
#define DBMGR_H

//#include "CctCfg.h"
//#include "CctMain.h"
#include "arch.h"
#include "sqlite3.h"
#include <map>
#include <string>

using namespace std;

class CDbMgr;
extern CDbMgr *g_pDbMgr;

#define ADDR_LEN 6
#define PARAM_DATA_LEN 8
#define PARAM_DATA_TIME_LEN 7

typedef struct StAmmeter
{
	int ammeterid;
	char addr[ADDR_LEN];					//���ַ
	char activepower[PARAM_DATA_LEN];		//�����й�
	char current[PARAM_DATA_LEN];			//����
	char voltage[PARAM_DATA_LEN];			//��ѹ
	char createtime[PARAM_DATA_TIME_LEN];	//����ʱ��
}ST_DB_AMMETER;


typedef std::map<int, string>  MP_AMMETER_ITEM_VALUE;

typedef struct StAmmeterParamInfo
{
    int ammeterid;
    int loopindex;
    string createtime;
    MP_AMMETER_ITEM_VALUE items;
}ST_AMMETER_PARAM;

typedef std::map<string, ST_AMMETER_PARAM> MP_AMMETER;


#if 0
//�����й��������й�����ѹ�����������ʣ���������......
typedef struct StAmmeter
{
	char addr[ADDR_LEN];					//���ַ
	int  ammeterid1;						//������
	char data1[PARAM_DATA_LEN];				
	int  ammeterid2;
	char data2[PARAM_DATA_LEN];				
	int  ammeterid3;
	char data3[PARAM_DATA_LEN];				
	int  ammeterid4;
	char data4[PARAM_DATA_LEN];				
	int  ammeterid5;
	char data5[PARAM_DATA_LEN];				
	int  ammeterid6;
	char data6[PARAM_DATA_LEN];				

	//����������
	//...
	
	char createtime[PARAM_DATA_TIME_LEN];	//����ʱ��
}ST_DB_AMMETER;
#endif




class CDbMgr
{
#if 0
public:
	CDbMgr();
	~CDbMgr();
	
	sqlite3* OpenDB(const char *dbname);				//�����ݿ�
	void CreateTables();								//�������ı��Լ�������
	void DropTables();									//ɾ�����ı��Լ�������

	int AddAmmeter(const char *szaddr);					//�����ı�����ӵ���ַ
	int GetAmmeterCount();								//��ȡ���ݱ��е������
	bool QueryAmmeter(const char *szaddr, int* id);		//��ѯ���ַ������
	int QueryAmmeters(ST_DB_AMMETER ammeter[], int nsize);	//��ѯ���ַ������

	int AddElectricity(const ST_DB_AMMETER* ammeter);	//�����ݵ���ַ�����������������
	int AddElectricitys(const ST_DB_AMMETER ammeter[], int nsize);//������������
	bool QueryElectricitys(ST_DB_AMMETER ammeter[], int nsize);//������ѯ���һ����������
	bool QueryLastElectricity(ST_DB_AMMETER* ammeter);	//��ѯ������һ�����������

	ST_DB_AMMETER * CreateAmmeters(int nsize);			//���������Ӧ���ݵ��ڴ�
	int GetAmmeterSize() const { return ammetersize; }	//�����ڴ��С

private:
	bool IsTableExists(const char *tablename);			//�����Ƿ����
	bool IsAmmeterExists(const char *szaddr, int* id);	//������ַ�Ƿ����
	bool IsAmmeterExists(ST_DB_AMMETER* ammeter);		//������ַ�Ƿ����
	bool QueryInt1(const char *sql, int* ret);			//���ڲ�ѯһ��int���͵ķ��ؽ������������ĳһ��idֵ��
	//bool QueryElectricity1(const char *sql, ST_DB_AMMETER* ammeter);
	bool QueryElectricitys(const char *sqlcondition, ST_DB_AMMETER ammeter[], int nszie);
	int CreateElectricityTB();							//����������
	int CreateAmmeterTB();								//�������ı�
	int ExeSql(const char* sql);						//ִ��sql���
	void Release();										//�ͷ�������ڴ�
	void CloseDB();

private:
	CDbMgr(const CDbMgr&);
	const CDbMgr& operator = (const CDbMgr&);

private:
	sqlite3 *db;
	ST_DB_AMMETER *ammeters;
	int ammetersize;



#else
public:
	CDbMgr();
	~CDbMgr();

	sqlite3* OpenDB(const char *dbname);				        //�����ݿ�
	void CreateTables();								        //�������ı��Լ�������
	void DropTables();									        //ɾ�����ı��Լ�������

    int AddAmmeters(MP_AMMETER &ammeters);                      //�����ı�����ӵ���ַ
    int AddAmmeter(const char *szaddr, int *id);                //�����ı�����ӵ�������ַ����ȡ����id
	int GetAmmeterCount();								        //��ȡ���ݱ��е������
	bool QueryAmmeter(const char *szaddr, int* id);		        //��ѯ���ַ������
    bool QueryAmmeters(MP_AMMETER &ammeters);	                //��ѯ���ַ������
    int UpdataAmmeters();                                       //����ammeters���е����ݵ����ݿ⵱��
    int UpdataAmmeters(MP_AMMETER &ammeters);                   //����ammeters���е����ݵ����ݿ⵱��
    bool QueryDataByTime(const char *szaddr, const char* sztime, int id, char * szout);    //ͨ�����ַ������id�Լ�ʱ���ѯ����

	int GetAmmeterSize() const { return ammetersize; }	        //�����ڴ��С
	int CreateElectricityTB(MP_AMMETER_ITEM_VALUE& itemmap);	//����������

	int AddAlarmInfo(char* alarminfo, int datalen);

    void InitTables();                                  //���������ݿ�֮���ʼ���������ݱ�
    int CreateAlarmTB();
    int InitAmmeterTB();                                //�������ݿ�֮���ʼ�����
    int InitElectricityTB();                            //�������ݿ�֮���ʼ��������ݱ��ÿ��������30�����ݣ���Ϊѭ������������
    int InitElectricityTB(const char* szaddr);          //ĳ��������֮���ʼ��������ݱ��ÿ��������30�����ݣ���Ϊѭ������������

    void AddAmmeterToMap(const char *szaddr);           //����ַ���뵽map��
    void RemoveAmmeterFromMap(const char *szaddr);      //�ѵ���map���Ƴ�
    void AddAmmeterDataToMap(const char *szaddr, int ammeterid, const char* szdata);    //������ݼ��뵽map��
    void AddAmmeterTimeToMap(const char *szaddr, const char* sztime);                   //������ݸ���ʱ��
    void RemoveAmmeterDataFromMap(const char *szaddr, int ammeterid);   //�ѵ�����ݴ�map���Ƴ�

private:
	bool IsTableExists(const char *tablename);			//�����Ƿ����
	bool IsAmmeterExists(const char *szaddr, int* id);	//������ַ�Ƿ����
	bool QueryInt1(const char *sql, int* ret);			//���ڲ�ѯһ��int���͵ķ��ؽ������������ĳһ��idֵ��
	//bool QueryElectricity1(const char *sql, ST_DB_AMMETER* ammeter);
	int AddAmmeter(const char *szaddr);					//�����ı�����ӵ�������ַ
	int ExeSql(const char* sql);						//ִ��sql���
    int InitElectricityTB(MP_AMMETER& ammeters);        //�������ݿ�֮���ʼ��������ݱ��ÿ��������30�����ݣ���Ϊѭ������������

	int CreateAmmeterTB();								//�������ı�
    string& InitUpdateElectricitySql(MP_AMMETER_ITEM_VALUE& itemmap, 
                                     const char* sztime, const char* szaddr);

	void CloseDB();                                     //�ر����ݿ�

private:
	CDbMgr(const CDbMgr&);
	const CDbMgr& operator = (const CDbMgr&);

private:
	sqlite3 *db;
    int ammetersize;
    string strupdateelectricitysql;
    MP_AMMETER mapammeter;

#endif

#if 1
public:
	//int DbMgrMsgProc(CCT_QUEUE_MSG_S *pstMsg);

	//void Init(TCctDbPara *pDbPara);
	void Run(void *pvArg);

	signed int DbMgrMsgProc(CCT_QUEUE_MSG_S *pstMsg);
	signed int DbMgrMsgProcEx(unsigned int dwMsgId, unsigned char *pInBuf, unsigned int dwInLen, unsigned char *pOutBuf, unsigned int dwOutLen);
	
	signed int SetPara(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize);
	signed int AddRecord(unsigned char *pFrmData, unsigned int dwFrmSize);
	
	signed int AddOneRecord(unsigned char *pMtrAddr, unsigned int dwID, unsigned char *pData, unsigned char bDataLen, TTime *pTime);
	signed int GetOneRecord(unsigned char *pAddr, unsigned int dwID, TTime *pStartTime, TTime *pEndTime, unsigned char *pOutBuf, unsigned int *pdwLen);
	
	signed int QueryItemData(unsigned char *pAddr, unsigned int dwID, TTime *pTime, unsigned int dwPoint, unsigned char *pOutBuf, unsigned int *pdwOutLen);
	signed int QueryUnReadItemForCctReader(unsigned char *pOutBuf, unsigned int *pdwOutLen);
	unsigned short Make645Frm(unsigned char* pbFrm, const unsigned char* pbAddr, 
										unsigned char bCmd, unsigned char* pData, unsigned char bDataLen);

	
	unsigned int GetIdFrom645Frm(unsigned char* pbFrm);
	unsigned int GetIntUSecond(unsigned int dwID);
	unsigned char GetMtrData(unsigned char* pData, unsigned char* pbFrm);
	
	signed int SaveItemData(unsigned char *pFrmData, unsigned int dwFrmSize);
	signed int SaveItemDataEx(unsigned char *pFrmData, unsigned int dwFrmSize);

	bool DoCctUnReadIdRep(unsigned char *pFrmData, unsigned int dwFrmSize);
	bool GetRecordTime(TTime *pTime, TTime *pStartTime, TTime *pEndTime);
	bool IsZoneAreaMtr(unsigned char *pAddr);
	bool GetMtrPara(unsigned short wMtrSn, unsigned char *pAddr);

	void DbMgrTaskKeepAlive();

private:
	//TCctDbPara *m_pDbPara;

	unsigned short m_wMtrSn;
	unsigned short m_wIDSn;
	
#endif


};




#endif


