//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : DbMgr.cpp
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

#include "DbMgr.h"
//#include "DbCfg.h"

//#include <stdlib.h>
#include <stdio.h>

CDbMgr *g_pDbMgr;
#if 0
#define SQL_DATA_LEN 512
const char INSERT_ELECTRICITY_SQL[] = "INSERT INTO tb_electricity (ammeterid, \
			activepower, current, voltage, createtime) VALUES (%d, '%s', '%s', '%s', '%s');";

#define SYS_WAIT_FOREVER			0xffffffff
#define CCT_DB_ADD_REC				0
#define CCT_DB_QUER_REC				1

CDbMgr::CDbMgr()
	: db(NULL)
	, ammeters(NULL)
	, ammetersize(0)
{
}

CDbMgr::~CDbMgr()
{
	CloseDB();
	Release();
}

void CDbMgr::CloseDB()
{
	if (db != NULL)
	{
		sqlite3_close(db);
		db = NULL;
	}
}

void CDbMgr::Release()
{
	if (ammeters != NULL)
	{
		delete[] ammeters;
		ammeters = NULL;
	}
}

ST_DB_AMMETER * CDbMgr::CreateAmmeters(int nsize)
{
	if (nsize > ammetersize)
	{
		Release();
		ammeters = new ST_DB_AMMETER[nsize];
	}
	return ammeters;
}

sqlite3* CDbMgr::OpenDB(const char *dbname)
{
	if (dbname != NULL)
	{
		CloseDB();
		if (sqlite3_open(dbname, &db))
		{
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return NULL;
		}
	}

	return db;
}


bool CDbMgr::IsTableExists(const char *tablename)
{
	if (db != NULL)
	{
		char sql[SQL_DATA_LEN] = { 0 };
		//sprintf_s(sql, sizeof(sql), "SELECT 1 FROM %s;", tablename);
		sprintf(sql, "SELECT 1 FROM %s;", tablename);
		return sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK;
	}
	return false;
}

int CDbMgr::CreateElectricityTB()
{
	const char *sql = "CREATE TABLE tb_electricity(\
                      id integer PRIMARY KEY AUTOINCREMENT,\
                      ammeterid UNSIGNED integer NOT NULL,\
                      activepower CHAR(8) NOT NULL,\
                      current CHAR(8) NOT NULL,\
                      voltage CHAR(8) NOT NULL,\
                      createtime CHAR(20) NOT NULL\
                    );";
	return ExeSql(sql);
}

int CDbMgr::CreateAmmeterTB()
{
	const char *sql = "CREATE TABLE tb_ammeter(\
                      id integer PRIMARY KEY AUTOINCREMENT,\
                      address CHAR(12) NOT NULL UNIQUE\
                    );";
	return ExeSql(sql);
}

int CDbMgr::ExeSql(const char* sql)
{
	if (db != NULL)
	{
		char *err = NULL;
		if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK)
		{
			sqlite3_free(err);
			return SQLITE_ERROR;
		}
		return SQLITE_OK;
	}

	return SQLITE_ERROR;
}

void CDbMgr::DropTables()
{
	const char *sqlammeter = "drop table if exists tb_ammeter;";
	ExeSql(sqlammeter);
	const char *sqldropelectricity = "drop table if exists tb_electricity;";
	ExeSql(sqldropelectricity);
}

bool CDbMgr::QueryInt1(const char *sql, int* ret)
{
	bool bfind = false;
	if (sql != NULL)
	{
		const char *zTail;
		sqlite3_stmt * stmt = NULL;
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, &zTail) == SQLITE_OK) {
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				*ret = sqlite3_column_int(stmt, 0);
				bfind = true;
				break;
			}
		}
		sqlite3_finalize(stmt);
	}

	return bfind;
}

bool CDbMgr::IsAmmeterExists(const char *szaddr, int* id)
{
	if (szaddr != NULL)
	{
		char sql[SQL_DATA_LEN] = { 0 };
		snprintf(sql, SQL_DATA_LEN, "SELECT id FROM tb_ammeter where address='%s';", szaddr);
		return QueryInt1(sql, id);
	}

	return false;
}

bool CDbMgr::IsAmmeterExists(ST_DB_AMMETER* ammeter)
{
	if (ammeter != NULL)
	{
		return IsAmmeterExists(ammeter->addr, &ammeter->ammeterid);
	}

	return false;
}

void CDbMgr::CreateTables()
{
	CreateAmmeterTB();
	CreateElectricityTB();
}

int CDbMgr::AddAmmeter(const char *szaddr)
{
	if (szaddr != NULL)
	{
		char sql[SQL_DATA_LEN] = { 0 };
		snprintf(sql, SQL_DATA_LEN, "INSERT INTO tb_ammeter (address) VALUES ('%s');", szaddr);

		return ExeSql(sql);
	}
	return SQLITE_ERROR;
}

int CDbMgr::QueryAmmeters(ST_DB_AMMETER ammeter[], int nsize)
{
	if (nsize > 0 && ammeter != NULL)
	{
		int i = 0;
		char * sztmp = NULL;
		char sql[] = "SELECT id , address FROM tb_ammeter ORDER BY id ASC;";
		const char *zTail;
		sqlite3_stmt * stmt = NULL;
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, &zTail) == SQLITE_OK) {
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				ammeter[i].ammeterid = sqlite3_column_int(stmt, 0);
				sztmp = (char*)sqlite3_column_text(stmt, 1);
				//memcpy_s(ammeter[i].addr, ADDR_LEN, sztmp, strlen(sztmp));
				memcpy(ammeter[i].addr, sztmp, strlen(sztmp));
				i++;
				if (i >= nsize)
				{
					break;
				}
			}
		}
		sqlite3_finalize(stmt);
	}
	return 0;
}

bool CDbMgr::QueryAmmeter(const char *szaddr, int* id)
{
	return IsAmmeterExists(szaddr, id);
}

int CDbMgr::AddElectricity(const ST_DB_AMMETER* ammeter)
{
	if (ammeter != NULL)
	{
		if (IsAmmeterExists(ammeter->addr, (int *)&ammeter->ammeterid))
		{
			char sql[SQL_DATA_LEN] = { 0 };
			snprintf(sql, SQL_DATA_LEN, INSERT_ELECTRICITY_SQL,
				ammeter->ammeterid, ammeter->activepower,
				ammeter->current, ammeter->voltage, ammeter->createtime
			);

			return ExeSql(sql);
		}
	}
	return SQLITE_ERROR;
}

int CDbMgr::AddElectricitys(const ST_DB_AMMETER ammeter[], int nsize)
{
	if (nsize > 0 && ammeter != NULL)
	{
		sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
		for (int i = 0; i < nsize; i++)
		{
			char sql[SQL_DATA_LEN] = { 0 };
			snprintf(sql, SQL_DATA_LEN, INSERT_ELECTRICITY_SQL, ammeter[i].ammeterid,
				ammeter[i].activepower, ammeter[i].current,
				ammeter[i].voltage, ammeter[i].createtime
			);
			sqlite3_exec(db, sql, NULL, NULL, NULL);
		}
		sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
	}
	return 0;
}

bool CDbMgr::QueryElectricitys(const char *sqlcondition, ST_DB_AMMETER ammeter[], int nszie)
{
	bool bsuccess = false;
	if ((ammeter != NULL) && (nszie > 0))
	{
		int i = 0;
		ST_DB_AMMETER *ptmp = ammeter;
		char * sztmp = NULL;
		const char *zTail;
		sqlite3_stmt * stmt = NULL;
		char sql[SQL_DATA_LEN] = { 0 };
		snprintf(sql, SQL_DATA_LEN,
			"SELECT ammeterid, activepower, current,  voltage, createtime, address FROM tb_electricity \
			INNER JOIN tb_ammeter ON tb_electricity.ammeterid = tb_ammeter.id %s;", sqlcondition);
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, &zTail) == SQLITE_OK) {
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				ptmp->ammeterid = sqlite3_column_int(stmt, 0);
				sztmp = (char*)sqlite3_column_text(stmt, 1);
				//memcpy_s(ptmp->activepower, PARAM_DATA_LEN, sztmp, strlen(sztmp));
				memcpy(ptmp->activepower, sztmp, strlen(sztmp));
				sztmp = (char*)sqlite3_column_text(stmt, 2);
				//memcpy_s(ptmp->current, PARAM_DATA_LEN, sztmp, strlen(sztmp));
				memcpy(ptmp->current, sztmp, strlen(sztmp));
				sztmp = (char*)sqlite3_column_text(stmt, 3);
				//memcpy_s(ptmp->voltage, PARAM_DATA_LEN, sztmp, strlen(sztmp));
				memcpy(ptmp->voltage, sztmp, strlen(sztmp));

				sztmp = (char*)sqlite3_column_text(stmt, 4);
				//memcpy_s(ptmp->createtime, PARAM_DATA_TIME_LEN, sztmp, strlen(sztmp));
				memcpy(ptmp->createtime, sztmp, strlen(sztmp));
				sztmp = (char*)sqlite3_column_text(stmt, 5);
				//memcpy_s(ptmp->addr, ADDR_LEN, sztmp, strlen(sztmp));
				memcpy(ptmp->addr, sztmp, strlen(sztmp));
				i++;
				ptmp++;
				if (i >= nszie)
				{
					bsuccess = true;
					break;
				}
			}
		}
		sqlite3_finalize(stmt);
	}

	return bsuccess;
}

bool CDbMgr::QueryElectricitys(ST_DB_AMMETER ammeter[], int nsize)
{
	return QueryElectricitys("ORDER BY ammeterid ASC", ammeter, nsize);
}

/*bool CDbMgr::QueryElectricity1(const char *sql, ST_DB_AMMETER* ammeter)
{
	bool bsuccess = false;
	if (ammeter != NULL)
	{
		int ammeterid = 0;
		if (IsAmmeterExists(ammeter->addr, (int *)&ammeter->ammeterid))
		{
			char * sztmp = NULL;
			char sql[SQL_DATA_LEN] = { 0 };
			snprintf(sql, sizeof(sql), "SELECT activepower, current, voltage, createtime \
				FROM tb_ammeter where id=%d %s;", ammeterid, sql);
			const char *zTail;
			sqlite3_stmt * stmt = NULL;
			if (sqlite3_prepare_v2(db, sql, -1, &stmt, &zTail) == SQLITE_OK) {
				while (sqlite3_step(stmt) == SQLITE_ROW) {
					sztmp = (char*)sqlite3_column_text(stmt, 0);
					memcpy_s(ammeter->activepower, PARAM_DATA_LEN, sztmp, strlen(sztmp));
					sztmp = (char*)sqlite3_column_text(stmt, 1);
					memcpy_s(ammeter->current, PARAM_DATA_LEN, sztmp, strlen(sztmp));
					sztmp = (char*)sqlite3_column_text(stmt, 2);
					memcpy_s(ammeter->voltage, PARAM_DATA_LEN, sztmp, strlen(sztmp));
					sztmp = (char*)sqlite3_column_text(stmt, 3);
					memcpy_s(ammeter->createtime, PARAM_DATA_TIME_LEN, sztmp, strlen(sztmp));
					bsuccess = true;
					break;
				}
			}
			sqlite3_finalize(stmt);
		}
	}

	return bsuccess;
}*/

bool CDbMgr::QueryLastElectricity(ST_DB_AMMETER* ammeter)
{
	if (ammeter != NULL)
	{
		if (IsAmmeterExists(ammeter->addr, (int *)&ammeter->ammeterid))
		{
			char sql[SQL_DATA_LEN] = { 0 };
			snprintf(sql, SQL_DATA_LEN, " WHERE ammeterid=%d ORDER BY tb_electricity.id ASC limit 1", ammeter->ammeterid);
			return QueryElectricitys(sql, ammeter, 1);
		}
	}

	return false;
}

int CDbMgr::GetAmmeterCount()
{
	int ncount = 0;
	char sql[] = "SELECT count() FROM tb_ammeter;";
	QueryInt1(sql, &ncount);

	return ncount;
}
#else
#define SQL_DATA_LEN 1024

const unsigned int LOOP_MAX = 30;

const char INSERT_ELECTRICITY_SQL[] = "INSERT INTO tb_electricity (ammeterid, activepower, current, voltage, createtime) \
                                       VALUES (%d, '%s', '%s', '%s', '%s');";
const char INSERT_AMMETER_ADDRESS_SQL[] = "INSERT INTO tb_ammeter (address) VALUES ('%s');";
const char INSERT_ELECTRICITY_LOOP_SQL[] = "INSERT INTO tb_electricity (ammeterid, loopindex, createtime) \
                                           SELECT id, %d, datetime()FROM tb_ammeter WHERE address = '%s';";
const char UPDATA_AMMETER_INDEX_SQL[] = "UPDATE tb_ammeter SET loopindex = %d WHERE address = '%s'";
const char INSERT_ALARMINFO_SQL[] = "INSERT INTO tb_alarm (info, datalength) VALUES ('%s', %d);";

CDbMgr::CDbMgr()
	: db(NULL)
	, ammetersize(0)
{
}

CDbMgr::~CDbMgr()
{
	CloseDB();
}

//****************************************************************************
// 功能描述： 关闭数据库
// 参数说明： 无
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::CloseDB()
{
	if (db != NULL)
	{
		sqlite3_close(db);
		db = NULL;
	}
}

//****************************************************************************
// 功能描述： 打开数据
// 参数说明： 数据库名称
// 返 回 值： 打开数据库sqlite3结构
//*****************************************************************************
sqlite3* CDbMgr::OpenDB(const char *dbname)
{
	if (dbname != NULL)
	{
		CloseDB();
		if (sqlite3_open(dbname, &db))
		{
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return NULL;
		}
	}

	return db;
}

//****************************************************************************
// 功能描述： 数据库表是否存在
// 参数说明： 数据库表的名称
// 返 回 值： 返回true表示所查询表存在
//            返回false表示所查询表不存在
//*****************************************************************************
bool CDbMgr::IsTableExists(const char *tablename)
{
	char sql[SQL_DATA_LEN] = { 0 };
	sprintf(sql, "SELECT 1 FROM %s;", tablename);
	return ExeSql(sql) == SQLITE_OK;
}

//****************************************************************************
// 功能描述： 根据itemmap项目创建tb_electricity表
//            电表对应的id，电表数据项-数据，更新的index，时间
// 参数说明： 初始化好的MP_AMMETER_ITEM_VALUE（电表数据项-数据）对象itemmap
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
// 使用说明： 此函数在创建好数据库并且已经初始化好MP_AMMETER_ITEM_VALUE
//            （电表数据项-数据）,并且此表不存在的时候调用
//*****************************************************************************
int CDbMgr::CreateElectricityTB(MP_AMMETER_ITEM_VALUE& itemmap)
{
	if (!itemmap.empty())
	{
		string str = "CREATE TABLE tb_electricity(\
                      id integer PRIMARY KEY AUTOINCREMENT,\
                      ammeterid UNSIGNED integer NOT NULL,\
                      loopindex UNSIGNED integer,";
		char buff[SQL_DATA_LEN] = { 0 };
		MP_AMMETER_ITEM_VALUE::iterator it = itemmap.begin();
		for (; it != itemmap.end(); it++)
		{
			memset(buff, 0, SQL_DATA_LEN);
			snprintf(buff, SQL_DATA_LEN, " '%08d' CHAR(%d),", it->first, it->second.length());
			str += buff;
		}
		str += "createtime CHAR(20) NOT NULL); ";
		return ExeSql(str.c_str());
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 根据itemmap、时间、电表地址创建更新语句
// 参数说明： MP_AMMETER_ITEM_VALUE（电表数据项-数据），更新时间sztime，电表
//            地址
// 返 回 值： 创建好的电表更新sql语句
//*****************************************************************************
string& CDbMgr::InitUpdateElectricitySql(MP_AMMETER_ITEM_VALUE& itemmap, const char* sztime, const char* szaddr)
{
	if (!itemmap.empty())
	{
		strupdateelectricitysql = "UPDATE tb_electricity SET ";
		char buff[SQL_DATA_LEN] = { 0 };
		MP_AMMETER_ITEM_VALUE::iterator it = itemmap.begin();
		for (; it != itemmap.end(); it++)
		{
			memset(buff, 0, SQL_DATA_LEN);
			snprintf(buff, SQL_DATA_LEN, " '%08d' = '%s',", it->first, it->second.c_str());
			strupdateelectricitysql += buff;
		}
		memset(buff, 0, SQL_DATA_LEN);
		snprintf(buff, SQL_DATA_LEN,
			" createtime = '%s' WHERE ammeterid = (SELECT id FROM tb_ammeter WHERE address = '%s') \
                AND loopindex = (SELECT loopindex FROM tb_ammeter WHERE address = '%s')",
			sztime, szaddr, szaddr);
		strupdateelectricitysql += buff;
	}

	return strupdateelectricitysql;
}

//****************************************************************************
// 功能描述： 创建tb_ammeter电表的表，存放电表地址，当前更新到的index（1-30）
//            自增id
// 参数说明： 无
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
// 使用说明： 此表不存在的时候调用
//*****************************************************************************
int CDbMgr::CreateAlarmTB()
{
	const char *sql = "CREATE TABLE tb_alarm(\
                      id integer PRIMARY KEY AUTOINCREMENT,\
                      datalength UNSIGNED integer DEFAULT 0,\
                      info CHAR(64) NOT NULL,\
                      createtime TIMESTAMP DEFAULT (datetime('now','localtime'))\
                    );";

	return ExeSql(sql);
}

//****************************************************************************
// 功能描述： 创建tb_ammeter电表的表，存放电表地址，当前更新到的index（1-30）
//            自增id
// 参数说明： 无
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
// 使用说明： 此表不存在的时候调用
//*****************************************************************************
int CDbMgr::CreateAmmeterTB()
{
	const char *sql = "CREATE TABLE tb_ammeter(\
                      id integer PRIMARY KEY AUTOINCREMENT,\
                      loopindex UNSIGNED integer DEFAULT 0,\
                      address CHAR(12) NOT NULL UNIQUE\
                    );";

	return ExeSql(sql);
}

//****************************************************************************
// 功能描述： 调用sqlite3的函数来执行sql语句
// 参数说明： 需要执行的sql语句
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::ExeSql(const char* sql)
{
	if (db != NULL)
	{
		char *err = NULL;
		if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK)
		{
			sqlite3_free(err);
			return SQLITE_ERROR;
		}
		return SQLITE_OK;
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 删除数据库表tb_ammeter表以及tb_electricity表
// 参数说明： 无
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::DropTables()
{
	if (db != NULL)
	{
		const char *sqlammeter = "drop table if exists tb_ammeter;";
		ExeSql(sqlammeter);
		const char *sqldropelectricity = "drop table if exists tb_electricity;";
		ExeSql(sqldropelectricity);
	}
}

//****************************************************************************
// 功能描述： 根据sql语句查询表中的一个int数据
// 参数说明： sql查询语句，int返回值
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
bool CDbMgr::QueryInt1(const char *sql, int* ret)
{
	bool bfind = false;
	if ((sql != NULL) && (db != NULL))
	{
		const char *zTail;
		sqlite3_stmt * stmt = NULL;
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, &zTail) == SQLITE_OK) {
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				*ret = sqlite3_column_int(stmt, 0);
				bfind = true;
				break;
			}
		}
		sqlite3_finalize(stmt);
	}

	return bfind;
}

//****************************************************************************
// 功能描述： 根据表地址查询电表是否存在，存在则返回对应的id值
// 参数说明： 电表的地址，int返回值
// 返 回 值： 返回false表示失败
//            返回true表示成功
//*****************************************************************************
bool CDbMgr::IsAmmeterExists(const char *szaddr, int* id)
{
	if (szaddr != NULL)
	{
		char sql[SQL_DATA_LEN] = { 0 };
		snprintf(sql, SQL_DATA_LEN, "SELECT id FROM tb_ammeter where address='%s';", szaddr);
		return QueryInt1(sql, id);
	}

	return false;
}

//****************************************************************************
// 功能描述： 创建电表以及电表相关数据的数据表
// 参数说明： 无
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::CreateTables()
{
	//if (!mapammeter.empty() && !IsTableExists("tb_ammeter") && !IsTableExists("tb_electricity"))
	if(!IsTableExists("tb_ammeter") && !IsTableExists("tb_electricity") && !IsTableExists("tb_alarm"))
	{
		CreateAlarmTB();
		CreateAmmeterTB();
		//CreateElectricityTB(mapammeter.begin()->second.items);
	}
}

//****************************************************************************
// 功能描述： 根据ammeters中所有电表地址添加电表中
// 参数说明： 电表数据集
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::AddAlarmInfo(char* alarminfo, int datalen)
{
	if((alarminfo != NULL) && (datalen > 0))
	{
		char sql[SQL_DATA_LEN] = { 0 };
		memset(sql, 0, SQL_DATA_LEN);
		snprintf(sql, SQL_DATA_LEN, INSERT_ALARMINFO_SQL, alarminfo, datalen);
		ExeSql(sql);
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 根据ammeters中所有电表地址添加电表中
// 参数说明： 电表数据集
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::AddAmmeters(MP_AMMETER &ammeters)
{
	if (!ammeters.empty() && (db != NULL))
	{
		char sql[SQL_DATA_LEN] = { 0 };
		if (ExeSql("BEGIN;") == SQLITE_ERROR)
		{
			return SQLITE_ERROR;
		}
		MP_AMMETER::iterator it = ammeters.begin();
		for (; it != ammeters.end(); it++)
		{
			if (IsAmmeterExists(it->first.c_str(), &it->second.ammeterid))
			{
				continue;
			}
			memset(sql, 0, SQL_DATA_LEN);
			snprintf(sql, SQL_DATA_LEN, INSERT_AMMETER_ADDRESS_SQL, it->first.c_str());
			ExeSql(sql);
		}
		ExeSql("COMMIT;");

		if (QueryAmmeters(ammeters))
		{
			return SQLITE_OK;
		}
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 将电表地址数据添加到电表中，并返回相应的id
// 参数说明： 电表地址，添加成功后返回电表id
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::AddAmmeter(const char *szaddr, int *id)
{
	if (AddAmmeter(szaddr) == SQLITE_OK)
	{
		return IsAmmeterExists(szaddr, id);
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 将电表地址数据添加到电表中
// 参数说明： 电表地址
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::AddAmmeter(const char *szaddr)
{
	if (szaddr != NULL)
	{
		char sql[SQL_DATA_LEN] = { 0 };
		snprintf(sql, SQL_DATA_LEN, "INSERT INTO tb_ammeter (address) VALUES ('%s');", szaddr);

		return ExeSql(sql);
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 查询电表中所有数据并放入ammeters当中
// 参数说明： 电表数据集
// 返 回 值： 返回false表示失败
//            返回true表示成功
//*****************************************************************************
bool CDbMgr::QueryAmmeters(MP_AMMETER &ammeters)
{
	bool bsucc = false;
	if (!ammeters.empty() && (db != NULL))
	{
		int i = 0;
		char * sztmp = NULL;
		char sql[] = "SELECT id, address, loopindex FROM tb_ammeter ORDER BY id ASC;";
		const char *zTail;
		sqlite3_stmt * stmt = NULL;
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, &zTail) == SQLITE_OK) {
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				sztmp = (char*)sqlite3_column_text(stmt, 1);
				ammeters[sztmp].ammeterid = sqlite3_column_int(stmt, 0);
				ammeters[sztmp].loopindex = sqlite3_column_int(stmt, 2);
				bsucc = true;
			}
		}
		sqlite3_finalize(stmt);
	}

	return bsucc;
}

//****************************************************************************
// 功能描述： 更新ammeters所有的数据到数据库当中
// 参数说明： 电表数据集
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::UpdataAmmeters()
{
	return UpdataAmmeters(mapammeter);
}

//****************************************************************************
// 功能描述： 更新ammeters所有的数据到数据库当中
// 参数说明： 电表数据集
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::UpdataAmmeters(MP_AMMETER &ammeters)
{
	if (!ammeters.empty())
	{
		char sql[SQL_DATA_LEN] = { 0 };
		if (ExeSql("BEGIN;") == SQLITE_ERROR)
		{
			return SQLITE_ERROR;
		}
		MP_AMMETER::iterator it = ammeters.begin();
		for (; it != ammeters.end(); it++)
		{
			InitUpdateElectricitySql(it->second.items, it->second.createtime.c_str(), it->first.c_str());
			ExeSql(strupdateelectricitysql.c_str());
			memset(sql, 0, SQL_DATA_LEN);
			if (++it->second.loopindex >= LOOP_MAX)
			{
				it->second.loopindex = 0;
			}
			snprintf(sql, SQL_DATA_LEN, UPDATA_AMMETER_INDEX_SQL, it->second.loopindex, it->first.c_str());
			ExeSql(sql);
		}
		return ExeSql("COMMIT;");
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 根据表地址查询电表是否存在，存在则返回对应的id值
// 参数说明： 电表的地址，int返回值
// 返 回 值： 返回false表示失败
//            返回true表示成功
//*****************************************************************************
bool CDbMgr::QueryAmmeter(const char *szaddr, int* id)
{
	return IsAmmeterExists(szaddr, id);
}

//****************************************************************************
// 功能描述： 获取电表数据总数
// 参数说明： 无
// 返 回 值： 电表数据总数
//*****************************************************************************
int CDbMgr::GetAmmeterCount()
{
	int ncount = 0;
	char sql[] = "SELECT count() FROM tb_ammeter;";
	QueryInt1(sql, &ncount);

	return ncount;
}

//****************************************************************************
// 功能描述： 初始化数电表以及数据表
// 参数说明： 无
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::InitTables()
{
	AddAmmeters(mapammeter);
	InitElectricityTB(mapammeter);
}

//****************************************************************************
// 功能描述： 初始化数电表数据项-数据
// 参数说明： 无
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::InitElectricityTB()
{
	return InitElectricityTB(mapammeter);
}

//****************************************************************************
// 功能描述： 初始化电表
// 参数说明： 无
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::InitAmmeterTB()
{
	return AddAmmeters(mapammeter);
}

//****************************************************************************
// 功能描述： 电表数据集中的每一个电表数据项-数据插入30条数据到数据表中
// 参数说明： 电表数据集
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::InitElectricityTB(MP_AMMETER& ammeters)
{
	if (!ammeters.empty() && (db != NULL))
	{
		char sql[SQL_DATA_LEN] = { 0 };
		sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);//未做判断，可能会存在问题
		MP_AMMETER::iterator it = ammeters.begin();
		for (; it != ammeters.end(); it++)
		{
			for (int i = 0; i < LOOP_MAX; i++)
			{
				memset(sql, 0, SQL_DATA_LEN);
				snprintf(sql, SQL_DATA_LEN, INSERT_ELECTRICITY_LOOP_SQL, i, it->first.c_str());
				sqlite3_exec(db, sql, NULL, NULL, NULL);
			}
		}
		sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);

		return SQLITE_OK;
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 电表数据项-数据插入30条数据到数据表中
// 参数说明： 电表地址
// 返 回 值： 返回SQLITE_ERROR表示失败
//            返回SQLITE_OK表示成功
//*****************************************************************************
int CDbMgr::InitElectricityTB(const char* szaddr)
{
	if ((szaddr != NULL) && (db != NULL))
	{
		char sql[SQL_DATA_LEN] = { 0 };
		sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);//未做判断，可能会存在问题
		for (int i = 0; i < LOOP_MAX; i++)
		{
			memset(sql, 0, SQL_DATA_LEN);
			snprintf(sql, SQL_DATA_LEN, INSERT_ELECTRICITY_LOOP_SQL, i, szaddr);
			sqlite3_exec(db, sql, NULL, NULL, NULL);
		}
		sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);

		return SQLITE_OK;
	}

	return SQLITE_ERROR;
}

//****************************************************************************
// 功能描述： 添加电表地址到电表数据集中
// 参数说明： 电表地址
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::AddAmmeterToMap(const char *szaddr)
{
	mapammeter[szaddr];
}

//****************************************************************************
// 功能描述： 从电表数据集中删除指定电表地址数据
// 参数说明： 电表地址
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::RemoveAmmeterFromMap(const char *szaddr)
{
	mapammeter.erase(szaddr);
}

//****************************************************************************
// 功能描述： 添加电表数据项-数据到数据集中
// 参数说明： 电表地址，电表数据项值，数据项对应的数据
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::AddAmmeterDataToMap(const char *szaddr, int ammeterid, const char* szdata)
{
	mapammeter[szaddr].items[ammeterid] = szdata;
}

//****************************************************************************
// 功能描述： 添加电表数据项-数据到数据集中
// 参数说明： 电表地址，电表数据项值，数据项对应的数据
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::AddAmmeterTimeToMap(const char *szaddr, const char* sztime)
{
	mapammeter[szaddr].createtime = sztime;
}

//****************************************************************************
// 功能描述： 从电表数据集中删除电表数据项
// 参数说明： 电表地址，电表数据项值
// 返 回 值： 无
//*****************************************************************************
void CDbMgr::RemoveAmmeterDataFromMap(const char *szaddr, int ammeterid)
{
	mapammeter[szaddr].items.erase(ammeterid);
}

//****************************************************************************
// 功能描述： 通过表地址，类型id以及时间查询数据
// 参数说明： 电表地址szaddr，电表数据项值szid，获取数值szout
// 返 回 值： 查询成功返回true，失败返回false
//*****************************************************************************
bool CDbMgr::QueryDataByTime(const char *szaddr, const char* sztime, int id, char * szout)
{	
	bool bsucc = false;
	if ((szaddr != NULL) && (sztime != NULL) && (szout != NULL) && (g_pDbMgr->db != NULL))
	{
		char buff[SQL_DATA_LEN] = { 0 };
		snprintf(buff, SQL_DATA_LEN, "select `%08d` from tb_electricity, tb_ammeter WHERE tb_ammeter.address = '%s'\
            AND tb_ammeter.id = tb_electricity.ammeterid AND createtime = '%s'",
			id, szaddr, sztime);
		const char *zTail;
		char * sztmp = NULL;
		sqlite3_stmt * stmt = NULL;
		if (sqlite3_prepare_v2(g_pDbMgr->db, buff, -1, &stmt, &zTail) == SQLITE_OK) {
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				sztmp = (char*)sqlite3_column_text(stmt, 0);
				memcpy(szout, sztmp, strlen(sztmp));
				bsucc = true;
			}
		}
		sqlite3_finalize(stmt);
	}

	return bsucc;
}

#endif

#if 0
//hfg-181210
//*****************************************************************************
// 函数名称: Init
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void CDbMgr::Init(TCctDbPara *pDbPara)
{
	m_wMtrSn = 0;
	m_wIDSn = 0;

	m_pDbPara = pDbPara;
	OpenDB((const char *)pDbPara->szDbName);
	CreateTables();
}


//*****************************************************************************
// 函数名称: Run
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void CDbMgr::Run(void *pvArg)
{
	TRACE("CDbMgrTask::Run().\n");

	unsigned char bType = 0;
	signed int dwRet = 0;
	APP_MSG_ID_ENUM dwMsgId;


	unsigned int dwRxLen = 0;
	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return;
	memset(pRxBuf, 0, APP_CCT_BUF_SIZE);


	unsigned int dwTxLen = 0;
	unsigned char *pTxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pTxBuf == NULL)return;
	memset(pTxBuf, 0, APP_CCT_BUF_SIZE);

	//注册定时器-用于查询提交抄表需求
	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_DB;
	tOnTimer.dwMsgId = APP_MSG_ID_SEC_ONTIMER;
	//秒定时任务
	//RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_DB;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_DBMGR_TASK_ALIVE;
	//线程的心跳检测
	RegisterOnTimer(&tOnTimer);


	for (;;)
	{
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_DB, &dwMsgId, pRxBuf, &dwRxLen, 0xffffffff))
		{
			//dwRet = DbMgrMsgProc(&stMsg);
			dwRet = DbMgrMsgProcEx(dwMsgId, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		}

		//Sleep(10);
	}
}

//*****************************************************************************
// 函数名称: DbMgrMsgProc
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::DbMgrMsgProc(CCT_QUEUE_MSG_S *pstMsg)
{
	return 0;
}

//*****************************************************************************
// 函数名称: DbMgrMsgProcEx
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::DbMgrMsgProcEx(unsigned int dwMsgId, unsigned char *pInBuf,
	unsigned int dwInLen, unsigned char *pOutBuf,
	unsigned int dwOutLen)
{
	signed int dwRet = 0;
	if ((NULL == pInBuf) ||
		(0 == dwInLen) ||
		(NULL == pOutBuf))
		return (-1);

	switch (dwMsgId)
	{

		//服务器直抄处理	
				//服务器过来的读数据请求,先查询数据库，默认读取最近一个点
				//有对应数据直接返回，没有对应数据提交抄表需求到CCT 
	case APP_MSG_ID_SVR_REQ:
		TRACE("DbMgrMsgProcEx Svr Direct Read.\n");
		if (CCT_ERR_SUCCESS == QueryItemData(GetMtrAddr(pInBuf), GetDataID(pInBuf, CCT_MTRPROTO_07),
			NULL, 1, pOutBuf, (unsigned int *)&dwOutLen))
		{
			//查询成功直接发回给服务器
			//AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pOutBuf, dwOutLen);
		}
		else
		{
			//提交抄表需求到CCT
			AppSendMessage2Queue(APP_QUEUE_ID_CCT, APP_MSG_ID_SVR_REQ, pInBuf, dwInLen);
		}

		break;

		//主站请求数据返回
	case APP_MSG_ID_SVR_REP:
		AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_645_DATA, pInBuf, dwInLen);
		break;


		//自动抄表处理
		//定时自动抄表 查询需要抄读的数据项 提交需求到CctReader
	case APP_MSG_ID_SEC_ONTIMER:
		QueryUnReadItemForCctReader(pOutBuf, &dwOutLen);
		break;

		//CctReader 主动路由请求抄读节点数据
	case APP_MSG_ID_CCT_REQ:
		DoCctUnReadIdRep(pInBuf, dwInLen);
		break;

		//抄表线程发过来的抄读数据
	case APP_MSG_ID_DB_REP:
		SaveItemData(pInBuf, dwInLen);
		break;



		//台区总表抄读处理
				//LTE通信线程过来的台区总表数据376.1透传数据
	case APP_MSG_ID_LTE_REP:
		SaveItemData(pInBuf, dwInLen);
		break;

	case APP_MSG_ID_CCT_DBMGR_TASK_ALIVE:
		DbMgrTaskKeepAlive();
		break;

	default:
		TRACE("DbMgrMsgProcEx undefine MsgId dwMsgId %d", dwMsgId);
		break;
	}

	return dwRet;
}


//*****************************************************************************
// 函数名称: DbMgrTaskKeepAlive
// 功能描述: 线程防止死机的检测
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
void CDbMgr::DbMgrTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_DBMGR_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}


#define MAX_POINT_NUM		1000
//*****************************************************************************
// 函数名称: QueryUnReadItemForCctReader
// 功能描述: 查询数据库并提交抄表需求到CctReader
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::QueryUnReadItemForCctReader(unsigned char *pOutBuf, unsigned int *pdwOutLen)
{
	TTime tTime;
	unsigned char bAddr[6] = {0x02,0x00,0x00,0x09,0x17,0x20};
	//GetMtrPara(m_wMtrSn, bAddr);

	if (m_wMtrSn < MAX_POINT_NUM)
	{
		if (m_wIDSn < GetIDCount())
		{
			GetTime(&tTime);

			//获取当前抄表间隔中一个应该抄读但未抄读的数据项
			if (CCT_ERR_SUCCESS != QueryItemData(bAddr, pCctRdCtrlTbl[m_wIDSn].dwID, &tTime, 1, pOutBuf, pdwOutLen))
			{
				*pdwOutLen = Make645Frm(pOutBuf, bAddr, 0x11, (unsigned char *)&pCctRdCtrlTbl[m_wIDSn].dwID, 4);

				if (IsZoneAreaMtr(bAddr))
				{
					//台区总表通过LTE通信维护线程用376.1协议透传抄读
					AppSendMessage2Queue(APP_QUEUE_ID_LTE, APP_MSG_ID_LTE_REQ, pOutBuf, *pdwOutLen);
				}
				else
				{
					//提交抄表需求到CctReader
					AppSendMessage2Queue(APP_QUEUE_ID_CCT, APP_MSG_ID_DB_REQ, pOutBuf, *pdwOutLen);
				}
			}

			m_wIDSn++;

		}
		else
		{
			m_wIDSn = 0;
			m_wMtrSn++;
		}
	}
	else
	{
		m_wMtrSn = 0;
		m_wIDSn = 0;	
	}
	
	return 0;

}

//*****************************************************************************
// 函数名称: IsZoneAreaMtr
// 功能描述: 台区总表判断
//
// 参数说明:电表地址、数据项ID、起始时间、 读取点数
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
bool CDbMgr::IsZoneAreaMtr(unsigned char *pAddr)
{
	return false;
}

//*****************************************************************************
// 函数名称: GetMtrPara
// 功能描述: 获取电表参数
//
// 参数说明:电表地址、数据项ID、起始时间、 读取点数
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
bool CDbMgr::GetMtrPara(unsigned short wMtrSn, unsigned char *pAddr)
{
	return true;
}

//*****************************************************************************
// 函数名称: QueryItemData
// 功能描述: 查询数据库中某项数据
//
// 参数说明:电表地址、数据项ID、起始时间、 读取点数
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::QueryItemData(unsigned char *pAddr, unsigned int dwID,
	TTime *pTime, unsigned int dwPoint,
	unsigned char *pOutBuf, unsigned int *pdwOutLen)
{
	TTime tStartTime = { 0 };
	TTime tEndTime = { 0 };
	unsigned int dwStartTimeSecond = 0;
	unsigned int dwEndTimeSecond = 0;
	unsigned int dwLen = 0;


	if ((pAddr == NULL) || (dwID == 0) || (dwPoint == 0))
	{
		return (-1);
	}

	if (pTime == NULL)
	{
		//没有指定时间，默认查询最近一次的数据
		GetTime(&tStartTime);
	}
	else
	{
		memcpy(&tStartTime, pTime, sizeof(TTime));
	}

	//if (TimeToSeconds(tStartTime) >= TimeToSeconds(*pTime))
	{
		dwStartTimeSecond = TimeToSeconds(tStartTime);

		for (unsigned int i = 0; i < dwPoint; i++)
		{
			dwEndTimeSecond = dwStartTimeSecond - GetIntUSecond(dwID);

			SecondsToTime(dwStartTimeSecond, &tStartTime);
			SecondsToTime(dwEndTimeSecond, &tEndTime);

			//依据电表地址、ID、起始结束时间读取一个点的数据
			if (CCT_ERR_SUCCESS == GetOneRecord(pAddr, dwID, &tStartTime, &tEndTime, pOutBuf, &dwLen))
			{
				pOutBuf += dwLen;
				*pdwOutLen += dwLen;
			}
			else
			{
				//全部读取成功才算OK，只要有1个点未读取到就返回失败
				return (-1);
				TRACE("tStartTime %s tEndTime %s Point Fail", &tStartTime, &tEndTime);
			}

			//读完后读取点-即下一个抄表间隔的数据
			dwStartTimeSecond = dwEndTimeSecond;
		}

	}

	return (-1);
}

//*****************************************************************************
// 函数名称: GetIntUSecond
// 功能描述: 获取一个抄读间隔所经历的秒
//
// 参数说明:电表地址、数据项ID、起始时间
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
unsigned int CDbMgr::GetIntUSecond(unsigned int dwID)
{
	unsigned char bIntU = GetCctIDIntVTable(dwID);
	unsigned int dwIntUSecond = 0;

	switch (bIntU)
	{
	case TIME_INTU_SEC:
		dwIntUSecond = 1;
		break;

	case TIME_INTU_MIN:
		dwIntUSecond = 60;
		break;

	case TIME_INTU_HOUR:
		dwIntUSecond = 3600;
		break;

	case TIME_INTU_DAY:
		dwIntUSecond = 86400;
		break;

	default:

		TRACE("GetIntUSecond default bType.\n");
		dwIntUSecond = 3600;
		break;

	}


	return dwIntUSecond;

}


//设置参数
//*****************************************************************************
// 函数名称: SetPara
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::SetPara(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize)
{
	return 0;
}

//*****************************************************************************
// 函数名称: SaveItemDataEx
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::SaveItemDataEx(unsigned char *pFrmData, unsigned int dwFrmSize)
{
	TRACE("SaveItemDataEx IsZoneAreaMtr.\n");



	//从3761中取出645帧，然后保存
	//return AddRecord(pFrmData, dwFrmSize);
	return 0;
}

//*****************************************************************************
// 函数名称: SaveItemData
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::SaveItemData(unsigned char *pFrmData, unsigned int dwFrmSize)
{
	return AddRecord(pFrmData, dwFrmSize);
}


//添加记录
//*****************************************************************************
// 函数名称: AddRecord
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::AddRecord(unsigned char *pFrmData, unsigned int dwFrmSize)
{
	if ((NULL == pFrmData) || (0 == dwFrmSize))
		return (-1);

	unsigned char bMtrAddr[6];
	unsigned int dwID;
	unsigned char bDataLen = 0;
	unsigned char bMtrProtocol = CCT_MTRPROTO_07;
	unsigned char bDataPos = 0;	
	unsigned char bMtrData[256];

	memset(bMtrAddr, 0, sizeof(bMtrAddr));
	memcpy(bMtrAddr, GetMtrAddr(pFrmData), sizeof(bMtrAddr));
	if (IsBufferAllZero(bMtrAddr, 6))
	{
		return (-1);
		TRACE("bMtrAddr Err.\n");
	}

	memcpy(bMtrAddr, pFrmData+1, 6);
	dwID = GetIdFrom645Frm(pFrmData);
	bDataLen = GetMtrData(bMtrData, pFrmData);

	TTime tTime = { 0 };
	GetTime(&tTime);

	return AddOneRecord(bMtrAddr, dwID, bMtrData, bDataLen, &tTime);

}

//添加电表记录
//*****************************************************************************
// 函数名称: AddOneRecord
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::AddOneRecord(unsigned char *pMtrAddr, unsigned int dwID,
	unsigned char *pData, unsigned char bDataLen,
	TTime *pTime)
{

	g_pDbMgr->OpenDB("CctData.db");
#if 0
	g_pDbMgr->AddAmmeter(pMtrAddr);
	memcpy(tMtrDataItem.addr, pMtrAddr, 6);
	tMtrDataItem.ammeterid = dwID;
	memcpy(tMtrDataItem.activepower, pData, 4);
	memcpy(tMtrDataItem.createtime, pTime, 16);
	g_pDbMgr->AddElectricity(tMtrDataItem);
#endif
	g_pDbMgr->CloseDB();
	return 0;
}


//读取数据
//*****************************************************************************
// 函数名称: GetOneRecord
// 功能描述: 
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
signed int CDbMgr::GetOneRecord(unsigned char *pAddr, unsigned int dwID,
										TTime *pStartTime, TTime *pEndTime,
										unsigned char *pOutBuf, unsigned int *pdwLen)
{
	TTime tTime;
	char bTime[16] = { 0 };
	char tTmptime[16] = { 0 };

	char bMtraAddr[16] = { 0 };
	char bData[256] = { 0 };

	//GetRecordTime(&tTime, pStartTime, pEndTime);
	GetTime(&tTime);
	*(unsigned short *)tTmptime = WordToBcd(tTime.wYear);
	*(unsigned char *)(tTmptime+2) = ByteToBcd(tTime.bMonth);
	*(unsigned char *)(tTmptime+3) = ByteToBcd(tTime.bDay);
	*(unsigned char *)(tTmptime+4) = ByteToBcd(tTime.bHour);
	*(unsigned char *)(tTmptime+5) = ByteToBcd(tTime.bMinute);
	*(unsigned char *)(tTmptime+6) = ByteToBcd(tTime.bSecond);

	HexToStr((char *)bMtraAddr, (char *)pAddr, strlen(bMtraAddr));
	HexToStr((char *)bTime, (char *)&tTmptime, strlen(bTime));

	if (true == QueryDataByTime(bMtraAddr, bTime, dwID, bData))
	{
		StrToHex((char *)pOutBuf, bData, strlen(bData));
		*pdwLen = strlen(bData) * 2;
		return CCT_ERR_SUCCESS;
	}
	else
	{
		return (-1);
	}
}

//*****************************************************************************
// 函数名称: GetRecordTime
// 功能描述: 获取数据的时间
//
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
bool CDbMgr::GetRecordTime(TTime *pTime, TTime *pStartTime, TTime *pEndTime)
{
	if (TimeToSeconds(*pStartTime) < TimeToSeconds(*pEndTime))
	{
		return false;
	}
	else if (TimeToSeconds(*pStartTime) == TimeToSeconds(*pEndTime))
	{
		pTime = pStartTime;
		return true;
	}
	else if (TimeToSeconds(*pStartTime) > TimeToSeconds(*pEndTime))
	{

		return false;
	}
}

//*****************************************************************************
// 函数名称: DoCctUnReadIdRep
// 功能描述: 主动路由的上报抄读帧处理，放在此处处理是因为
//			需要查询数据库才知道是否需要抄读，抄读什么数据
// 参数说明:
//  This function has no arguments.
//
// 返 回 值:
//    无
//
// 调用要求: TODO: ...
// 调用举例: TODO: ...
// 作    者: sUesTcxF
//*****************************************************************************
bool CDbMgr::DoCctUnReadIdRep(unsigned char *pFrmData, unsigned int dwFrmSize)
{
	return false;
}



unsigned short CDbMgr::Make645Frm(unsigned char* pbFrm, const unsigned char* pbAddr, 
										unsigned char bCmd, unsigned char* pData, unsigned char bDataLen)
{
	pbFrm[0] = 0x68;
	memcpy(&pbFrm[1], pbAddr, 6);
	pbFrm[7] = 0x68;
	pbFrm[8] = bCmd;
	pbFrm[9] = bDataLen;
	memcpy(&pbFrm[10], pData, bDataLen);

    for (unsigned short i=10; i<(unsigned short)bDataLen+10; i++)
	{
  	    pbFrm[i] += 0x33;
	}	 
	
	pbFrm[10+(unsigned short)bDataLen] = CheckSum(pbFrm, (unsigned short)bDataLen+10);
	pbFrm[11+(unsigned short)bDataLen] = 0x16;

	return bDataLen+12;
}  

unsigned int CDbMgr::GetIdFrom645Frm(unsigned char* pbFrm)
{
	unsigned int dwID = 0;
	unsigned char* p = (unsigned char *)&dwID;
	if (pbFrm[8] == 0x91) //07协议
	{
		*p++ = pbFrm[10]-0x33;
		*p++ = pbFrm[11]-0x33;
		*p++ = pbFrm[12]-0x33;
		*p = pbFrm[13]-0x33;
	}
	else	//97协议
	{
		*p++ = pbFrm[10]-0x33;
		*p++ = pbFrm[11]-0x33;
	}

	return dwID;
}

unsigned char CDbMgr::GetMtrData(unsigned char* pData, unsigned char* pbFrm)
{

	if (pbFrm[8] == 0x91) //07协议
	{
		for (int i = 0; i < *(pbFrm+9)-4; i++)
		{
			*pData++ = pbFrm[14+i]-0x33;
		}

		return *(pbFrm + 9) - 4;
	}
	else	//97协议
	{
		for (int i = 0; i < *(pbFrm+9)-2; i++)
		{
			*pData++ = pbFrm[12+i]-0x33;
		}

		return *(pbFrm + 9) - 2;
	}

	
}

#endif
