//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : DbMgr.h
//  版 本 号   : V1.0
//  作    者   : tjl
//  生成日期   : 2018-10-31
//  功能描述   : 
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-31
//    作    者 : sUesTcxF
//    修改内容 : 
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
	char addr[ADDR_LEN];					//表地址
	char activepower[PARAM_DATA_LEN];		//正向有功
	char current[PARAM_DATA_LEN];			//电流
	char voltage[PARAM_DATA_LEN];			//电压
	char createtime[PARAM_DATA_TIME_LEN];	//更新时间
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
//正向有功，反向有功，电压，电流，功率，功率因素......
typedef struct StAmmeter
{
	char addr[ADDR_LEN];					//表地址
	int  ammeterid1;						//数据项
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

	//增加数据项
	//...
	
	char createtime[PARAM_DATA_TIME_LEN];	//更新时间
}ST_DB_AMMETER;
#endif




class CDbMgr
{
#if 0
public:
	CDbMgr();
	~CDbMgr();
	
	sqlite3* OpenDB(const char *dbname);				//打开数据库
	void CreateTables();								//创建电表的表以及电力表
	void DropTables();									//删除电表的表以及电力表

	int AddAmmeter(const char *szaddr);					//往电表的表中添加电表地址
	int GetAmmeterCount();								//获取数据表中电表数量
	bool QueryAmmeter(const char *szaddr, int* id);		//查询表地址中数据
	int QueryAmmeters(ST_DB_AMMETER ammeter[], int nsize);	//查询表地址中数据

	int AddElectricity(const ST_DB_AMMETER* ammeter);	//往根据电表地址往电力表中添加数据
	int AddElectricitys(const ST_DB_AMMETER ammeter[], int nsize);//批量增加数据
	bool QueryElectricitys(ST_DB_AMMETER ammeter[], int nsize);//批量查询最后一条插入数据
	bool QueryLastElectricity(ST_DB_AMMETER* ammeter);	//查询电表最后一条插入的数据

	ST_DB_AMMETER * CreateAmmeters(int nsize);			//申请电表和相应数据的内存
	int GetAmmeterSize() const { return ammetersize; }	//申请内存大小

private:
	bool IsTableExists(const char *tablename);			//检查表是否存在
	bool IsAmmeterExists(const char *szaddr, int* id);	//检查电表地址是否存在
	bool IsAmmeterExists(ST_DB_AMMETER* ammeter);		//检查电表地址是否存在
	bool QueryInt1(const char *sql, int* ret);			//用于查询一个int类型的返回结果，如总数或某一个id值等
	//bool QueryElectricity1(const char *sql, ST_DB_AMMETER* ammeter);
	bool QueryElectricitys(const char *sqlcondition, ST_DB_AMMETER ammeter[], int nszie);
	int CreateElectricityTB();							//创建电力表
	int CreateAmmeterTB();								//创建电表的表
	int ExeSql(const char* sql);						//执行sql语句
	void Release();										//释放申请的内存
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

	sqlite3* OpenDB(const char *dbname);				        //打开数据库
	void CreateTables();								        //创建电表的表以及电力表
	void DropTables();									        //删除电表的表以及电力表

    int AddAmmeters(MP_AMMETER &ammeters);                      //往电表的表中添加电表地址
    int AddAmmeter(const char *szaddr, int *id);                //往电表的表中添加单个电表地址并获取插入id
	int GetAmmeterCount();								        //获取数据表中电表数量
	bool QueryAmmeter(const char *szaddr, int* id);		        //查询表地址中数据
    bool QueryAmmeters(MP_AMMETER &ammeters);	                //查询表地址中数据
    int UpdataAmmeters();                                       //更新ammeters所有的数据到数据库当中
    int UpdataAmmeters(MP_AMMETER &ammeters);                   //更新ammeters所有的数据到数据库当中
    bool QueryDataByTime(const char *szaddr, const char* sztime, int id, char * szout);    //通过表地址，类型id以及时间查询数据

	int GetAmmeterSize() const { return ammetersize; }	        //申请内存大小
	int CreateElectricityTB(MP_AMMETER_ITEM_VALUE& itemmap);	//创建电力表

	int AddAlarmInfo(char* alarminfo, int datalen);

    void InitTables();                                  //创建好数据库之后初始化电表和数据表
    int CreateAlarmTB();
    int InitAmmeterTB();                                //创建数据库之后初始化电表
    int InitElectricityTB();                            //创建数据库之后初始化电表数据表项，每个电表插入30条数据，作为循环更新数据用
    int InitElectricityTB(const char* szaddr);          //某个电表添加之后初始化电表数据表项，每个电表插入30条数据，作为循环更新数据用

    void AddAmmeterToMap(const char *szaddr);           //电表地址加入到map中
    void RemoveAmmeterFromMap(const char *szaddr);      //把电表从map中移除
    void AddAmmeterDataToMap(const char *szaddr, int ammeterid, const char* szdata);    //电表数据加入到map中
    void AddAmmeterTimeToMap(const char *szaddr, const char* sztime);                   //电表数据更新时间
    void RemoveAmmeterDataFromMap(const char *szaddr, int ammeterid);   //把电表数据从map中移除

private:
	bool IsTableExists(const char *tablename);			//检查表是否存在
	bool IsAmmeterExists(const char *szaddr, int* id);	//检查电表地址是否存在
	bool QueryInt1(const char *sql, int* ret);			//用于查询一个int类型的返回结果，如总数或某一个id值等
	//bool QueryElectricity1(const char *sql, ST_DB_AMMETER* ammeter);
	int AddAmmeter(const char *szaddr);					//往电表的表中添加单个电表地址
	int ExeSql(const char* sql);						//执行sql语句
    int InitElectricityTB(MP_AMMETER& ammeters);        //创建数据库之后初始化电表数据表项，每个电表插入30条数据，作为循环更新数据用

	int CreateAmmeterTB();								//创建电表的表
    string& InitUpdateElectricitySql(MP_AMMETER_ITEM_VALUE& itemmap, 
                                     const char* sztime, const char* szaddr);

	void CloseDB();                                     //关闭数据库

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


