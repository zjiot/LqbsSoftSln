//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : DbMgr.cpp
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
// ���������� �ر����ݿ�
// ����˵���� ��
// �� �� ֵ�� ��
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
// ���������� ������
// ����˵���� ���ݿ�����
// �� �� ֵ�� �����ݿ�sqlite3�ṹ
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
// ���������� ���ݿ���Ƿ����
// ����˵���� ���ݿ�������
// �� �� ֵ�� ����true��ʾ����ѯ�����
//            ����false��ʾ����ѯ������
//*****************************************************************************
bool CDbMgr::IsTableExists(const char *tablename)
{
	char sql[SQL_DATA_LEN] = { 0 };
	sprintf(sql, "SELECT 1 FROM %s;", tablename);
	return ExeSql(sql) == SQLITE_OK;
}

//****************************************************************************
// ���������� ����itemmap��Ŀ����tb_electricity��
//            ����Ӧ��id�����������-���ݣ����µ�index��ʱ��
// ����˵���� ��ʼ���õ�MP_AMMETER_ITEM_VALUE�����������-���ݣ�����itemmap
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
// ʹ��˵���� �˺����ڴ��������ݿⲢ���Ѿ���ʼ����MP_AMMETER_ITEM_VALUE
//            �����������-���ݣ�,���Ҵ˱����ڵ�ʱ�����
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
// ���������� ����itemmap��ʱ�䡢����ַ�����������
// ����˵���� MP_AMMETER_ITEM_VALUE�����������-���ݣ�������ʱ��sztime�����
//            ��ַ
// �� �� ֵ�� �����õĵ�����sql���
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
// ���������� ����tb_ammeter���ı���ŵ���ַ����ǰ���µ���index��1-30��
//            ����id
// ����˵���� ��
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
// ʹ��˵���� �˱����ڵ�ʱ�����
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
// ���������� ����tb_ammeter���ı���ŵ���ַ����ǰ���µ���index��1-30��
//            ����id
// ����˵���� ��
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
// ʹ��˵���� �˱����ڵ�ʱ�����
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
// ���������� ����sqlite3�ĺ�����ִ��sql���
// ����˵���� ��Ҫִ�е�sql���
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
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
// ���������� ɾ�����ݿ��tb_ammeter���Լ�tb_electricity��
// ����˵���� ��
// �� �� ֵ�� ��
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
// ���������� ����sql����ѯ���е�һ��int����
// ����˵���� sql��ѯ��䣬int����ֵ
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
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
// ���������� ���ݱ��ַ��ѯ����Ƿ���ڣ������򷵻ض�Ӧ��idֵ
// ����˵���� ���ĵ�ַ��int����ֵ
// �� �� ֵ�� ����false��ʾʧ��
//            ����true��ʾ�ɹ�
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
// ���������� ��������Լ����������ݵ����ݱ�
// ����˵���� ��
// �� �� ֵ�� ��
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
// ���������� ����ammeters�����е���ַ��ӵ����
// ����˵���� ������ݼ�
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
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
// ���������� ����ammeters�����е���ַ��ӵ����
// ����˵���� ������ݼ�
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
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
// ���������� ������ַ������ӵ�����У���������Ӧ��id
// ����˵���� ����ַ����ӳɹ��󷵻ص��id
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
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
// ���������� ������ַ������ӵ������
// ����˵���� ����ַ
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
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
// ���������� ��ѯ������������ݲ�����ammeters����
// ����˵���� ������ݼ�
// �� �� ֵ�� ����false��ʾʧ��
//            ����true��ʾ�ɹ�
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
// ���������� ����ammeters���е����ݵ����ݿ⵱��
// ����˵���� ������ݼ�
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
//*****************************************************************************
int CDbMgr::UpdataAmmeters()
{
	return UpdataAmmeters(mapammeter);
}

//****************************************************************************
// ���������� ����ammeters���е����ݵ����ݿ⵱��
// ����˵���� ������ݼ�
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
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
// ���������� ���ݱ��ַ��ѯ����Ƿ���ڣ������򷵻ض�Ӧ��idֵ
// ����˵���� ���ĵ�ַ��int����ֵ
// �� �� ֵ�� ����false��ʾʧ��
//            ����true��ʾ�ɹ�
//*****************************************************************************
bool CDbMgr::QueryAmmeter(const char *szaddr, int* id)
{
	return IsAmmeterExists(szaddr, id);
}

//****************************************************************************
// ���������� ��ȡ�����������
// ����˵���� ��
// �� �� ֵ�� �����������
//*****************************************************************************
int CDbMgr::GetAmmeterCount()
{
	int ncount = 0;
	char sql[] = "SELECT count() FROM tb_ammeter;";
	QueryInt1(sql, &ncount);

	return ncount;
}

//****************************************************************************
// ���������� ��ʼ��������Լ����ݱ�
// ����˵���� ��
// �� �� ֵ�� ��
//*****************************************************************************
void CDbMgr::InitTables()
{
	AddAmmeters(mapammeter);
	InitElectricityTB(mapammeter);
}

//****************************************************************************
// ���������� ��ʼ�������������-����
// ����˵���� ��
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
//*****************************************************************************
int CDbMgr::InitElectricityTB()
{
	return InitElectricityTB(mapammeter);
}

//****************************************************************************
// ���������� ��ʼ�����
// ����˵���� ��
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
//*****************************************************************************
int CDbMgr::InitAmmeterTB()
{
	return AddAmmeters(mapammeter);
}

//****************************************************************************
// ���������� ������ݼ��е�ÿһ�����������-���ݲ���30�����ݵ����ݱ���
// ����˵���� ������ݼ�
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
//*****************************************************************************
int CDbMgr::InitElectricityTB(MP_AMMETER& ammeters)
{
	if (!ammeters.empty() && (db != NULL))
	{
		char sql[SQL_DATA_LEN] = { 0 };
		sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);//δ���жϣ����ܻ��������
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
// ���������� ���������-���ݲ���30�����ݵ����ݱ���
// ����˵���� ����ַ
// �� �� ֵ�� ����SQLITE_ERROR��ʾʧ��
//            ����SQLITE_OK��ʾ�ɹ�
//*****************************************************************************
int CDbMgr::InitElectricityTB(const char* szaddr)
{
	if ((szaddr != NULL) && (db != NULL))
	{
		char sql[SQL_DATA_LEN] = { 0 };
		sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);//δ���жϣ����ܻ��������
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
// ���������� ��ӵ���ַ��������ݼ���
// ����˵���� ����ַ
// �� �� ֵ�� ��
//*****************************************************************************
void CDbMgr::AddAmmeterToMap(const char *szaddr)
{
	mapammeter[szaddr];
}

//****************************************************************************
// ���������� �ӵ�����ݼ���ɾ��ָ������ַ����
// ����˵���� ����ַ
// �� �� ֵ�� ��
//*****************************************************************************
void CDbMgr::RemoveAmmeterFromMap(const char *szaddr)
{
	mapammeter.erase(szaddr);
}

//****************************************************************************
// ���������� ��ӵ��������-���ݵ����ݼ���
// ����˵���� ����ַ�����������ֵ���������Ӧ������
// �� �� ֵ�� ��
//*****************************************************************************
void CDbMgr::AddAmmeterDataToMap(const char *szaddr, int ammeterid, const char* szdata)
{
	mapammeter[szaddr].items[ammeterid] = szdata;
}

//****************************************************************************
// ���������� ��ӵ��������-���ݵ����ݼ���
// ����˵���� ����ַ�����������ֵ���������Ӧ������
// �� �� ֵ�� ��
//*****************************************************************************
void CDbMgr::AddAmmeterTimeToMap(const char *szaddr, const char* sztime)
{
	mapammeter[szaddr].createtime = sztime;
}

//****************************************************************************
// ���������� �ӵ�����ݼ���ɾ�����������
// ����˵���� ����ַ�����������ֵ
// �� �� ֵ�� ��
//*****************************************************************************
void CDbMgr::RemoveAmmeterDataFromMap(const char *szaddr, int ammeterid)
{
	mapammeter[szaddr].items.erase(ammeterid);
}

//****************************************************************************
// ���������� ͨ�����ַ������id�Լ�ʱ���ѯ����
// ����˵���� ����ַszaddr�����������ֵszid����ȡ��ֵszout
// �� �� ֵ�� ��ѯ�ɹ�����true��ʧ�ܷ���false
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
// ��������: Init
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: Run
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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

	//ע�ᶨʱ��-���ڲ�ѯ�ύ��������
	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_DB;
	tOnTimer.dwMsgId = APP_MSG_ID_SEC_ONTIMER;
	//�붨ʱ����
	//RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_DB;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_DBMGR_TASK_ALIVE;
	//�̵߳��������
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
// ��������: DbMgrMsgProc
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed int CDbMgr::DbMgrMsgProc(CCT_QUEUE_MSG_S *pstMsg)
{
	return 0;
}

//*****************************************************************************
// ��������: DbMgrMsgProcEx
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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

		//������ֱ������	
				//�����������Ķ���������,�Ȳ�ѯ���ݿ⣬Ĭ�϶�ȡ���һ����
				//�ж�Ӧ����ֱ�ӷ��أ�û�ж�Ӧ�����ύ��������CCT 
	case APP_MSG_ID_SVR_REQ:
		TRACE("DbMgrMsgProcEx Svr Direct Read.\n");
		if (CCT_ERR_SUCCESS == QueryItemData(GetMtrAddr(pInBuf), GetDataID(pInBuf, CCT_MTRPROTO_07),
			NULL, 1, pOutBuf, (unsigned int *)&dwOutLen))
		{
			//��ѯ�ɹ�ֱ�ӷ��ظ�������
			//AppSendMessage2Queue(APP_QUEUE_ID_DRV, APP_MSG_ID_SVR_REP, pOutBuf, dwOutLen);
		}
		else
		{
			//�ύ��������CCT
			AppSendMessage2Queue(APP_QUEUE_ID_CCT, APP_MSG_ID_SVR_REQ, pInBuf, dwInLen);
		}

		break;

		//��վ�������ݷ���
	case APP_MSG_ID_SVR_REP:
		AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REP_645_DATA, pInBuf, dwInLen);
		break;


		//�Զ�������
		//��ʱ�Զ����� ��ѯ��Ҫ������������ �ύ����CctReader
	case APP_MSG_ID_SEC_ONTIMER:
		QueryUnReadItemForCctReader(pOutBuf, &dwOutLen);
		break;

		//CctReader ����·�����󳭶��ڵ�����
	case APP_MSG_ID_CCT_REQ:
		DoCctUnReadIdRep(pInBuf, dwInLen);
		break;

		//�����̷߳������ĳ�������
	case APP_MSG_ID_DB_REP:
		SaveItemData(pInBuf, dwInLen);
		break;



		//̨���ܱ�������
				//LTEͨ���̹߳�����̨���ܱ�����376.1͸������
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
// ��������: DbMgrTaskKeepAlive
// ��������: �̷߳�ֹ�����ļ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
void CDbMgr::DbMgrTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_DBMGR_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}


#define MAX_POINT_NUM		1000
//*****************************************************************************
// ��������: QueryUnReadItemForCctReader
// ��������: ��ѯ���ݿⲢ�ύ��������CctReader
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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

			//��ȡ��ǰ��������һ��Ӧ�ó�����δ������������
			if (CCT_ERR_SUCCESS != QueryItemData(bAddr, pCctRdCtrlTbl[m_wIDSn].dwID, &tTime, 1, pOutBuf, pdwOutLen))
			{
				*pdwOutLen = Make645Frm(pOutBuf, bAddr, 0x11, (unsigned char *)&pCctRdCtrlTbl[m_wIDSn].dwID, 4);

				if (IsZoneAreaMtr(bAddr))
				{
					//̨���ܱ�ͨ��LTEͨ��ά���߳���376.1Э��͸������
					AppSendMessage2Queue(APP_QUEUE_ID_LTE, APP_MSG_ID_LTE_REQ, pOutBuf, *pdwOutLen);
				}
				else
				{
					//�ύ��������CctReader
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
// ��������: IsZoneAreaMtr
// ��������: ̨���ܱ��ж�
//
// ����˵��:����ַ��������ID����ʼʱ�䡢 ��ȡ����
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
bool CDbMgr::IsZoneAreaMtr(unsigned char *pAddr)
{
	return false;
}

//*****************************************************************************
// ��������: GetMtrPara
// ��������: ��ȡ������
//
// ����˵��:����ַ��������ID����ʼʱ�䡢 ��ȡ����
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
bool CDbMgr::GetMtrPara(unsigned short wMtrSn, unsigned char *pAddr)
{
	return true;
}

//*****************************************************************************
// ��������: QueryItemData
// ��������: ��ѯ���ݿ���ĳ������
//
// ����˵��:����ַ��������ID����ʼʱ�䡢 ��ȡ����
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
		//û��ָ��ʱ�䣬Ĭ�ϲ�ѯ���һ�ε�����
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

			//���ݵ���ַ��ID����ʼ����ʱ���ȡһ���������
			if (CCT_ERR_SUCCESS == GetOneRecord(pAddr, dwID, &tStartTime, &tEndTime, pOutBuf, &dwLen))
			{
				pOutBuf += dwLen;
				*pdwOutLen += dwLen;
			}
			else
			{
				//ȫ����ȡ�ɹ�����OK��ֻҪ��1����δ��ȡ���ͷ���ʧ��
				return (-1);
				TRACE("tStartTime %s tEndTime %s Point Fail", &tStartTime, &tEndTime);
			}

			//������ȡ��-����һ��������������
			dwStartTimeSecond = dwEndTimeSecond;
		}

	}

	return (-1);
}

//*****************************************************************************
// ��������: GetIntUSecond
// ��������: ��ȡһ�������������������
//
// ����˵��:����ַ��������ID����ʼʱ��
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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


//���ò���
//*****************************************************************************
// ��������: SetPara
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed int CDbMgr::SetPara(unsigned int dwMsgId, unsigned char *pFrmData, unsigned int dwFrmSize)
{
	return 0;
}

//*****************************************************************************
// ��������: SaveItemDataEx
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed int CDbMgr::SaveItemDataEx(unsigned char *pFrmData, unsigned int dwFrmSize)
{
	TRACE("SaveItemDataEx IsZoneAreaMtr.\n");



	//��3761��ȡ��645֡��Ȼ�󱣴�
	//return AddRecord(pFrmData, dwFrmSize);
	return 0;
}

//*****************************************************************************
// ��������: SaveItemData
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
//*****************************************************************************
signed int CDbMgr::SaveItemData(unsigned char *pFrmData, unsigned int dwFrmSize)
{
	return AddRecord(pFrmData, dwFrmSize);
}


//��Ӽ�¼
//*****************************************************************************
// ��������: AddRecord
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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

//��ӵ���¼
//*****************************************************************************
// ��������: AddOneRecord
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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


//��ȡ����
//*****************************************************************************
// ��������: GetOneRecord
// ��������: 
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: GetRecordTime
// ��������: ��ȡ���ݵ�ʱ��
//
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
// ��������: DoCctUnReadIdRep
// ��������: ����·�ɵ��ϱ�����֡�������ڴ˴���������Ϊ
//			��Ҫ��ѯ���ݿ��֪���Ƿ���Ҫ����������ʲô����
// ����˵��:
//  This function has no arguments.
//
// �� �� ֵ:
//    ��
//
// ����Ҫ��: TODO: ...
// ���þ���: TODO: ...
// ��    ��: sUesTcxF
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
	if (pbFrm[8] == 0x91) //07Э��
	{
		*p++ = pbFrm[10]-0x33;
		*p++ = pbFrm[11]-0x33;
		*p++ = pbFrm[12]-0x33;
		*p = pbFrm[13]-0x33;
	}
	else	//97Э��
	{
		*p++ = pbFrm[10]-0x33;
		*p++ = pbFrm[11]-0x33;
	}

	return dwID;
}

unsigned char CDbMgr::GetMtrData(unsigned char* pData, unsigned char* pbFrm)
{

	if (pbFrm[8] == 0x91) //07Э��
	{
		for (int i = 0; i < *(pbFrm+9)-4; i++)
		{
			*pData++ = pbFrm[14+i]-0x33;
		}

		return *(pbFrm + 9) - 4;
	}
	else	//97Э��
	{
		for (int i = 0; i < *(pbFrm+9)-2; i++)
		{
			*pData++ = pbFrm[12+i]-0x33;
		}

		return *(pbFrm + 9) - 2;
	}

	
}

#endif
