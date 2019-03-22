//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : CctMain.cpp
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

#include "LgtMain.h"
#include "LgtProtocol.h"
#include "LgtMrsProtocol.h"
#include "arch.h"
#include "Comm.h"
typedef struct {
	uint8_t buf[512];
	uint16_t len;
} LgtBuffer;

typedef enum {
	LGT_STATUS_IDLE = 0,
	LGT_STATUS_LOGINING,
	LGT_STATUS_HEART,
} LgtStatusEnum;

typedef struct {
	uint8_t id;
	uint8_t macAddr[LGT_MAC_ADDRESS_LEN];
	uint8_t servIp[4];
	uint32_t servPort;
	LgtStatusEnum status;
	TTOnTimer loginTimer;
	TTOnTimer heartTimer;
	CComm comm;
} LgtContext;

/*
#define LGT_LOGIN_RESEND_INTERVAL		10
#define LGT_HEART_INTERVAL				30
#define LGT_DNS_IP						"127.0.0.1"
#define LGT_IP_0						39
#define LGT_IP_1						104
#define LGT_IP_2						62
#define LGT_IP_3						164
#define LGT_PORT						6060
*/

#define LGT_IP_0						127
#define LGT_IP_1						0
#define LGT_IP_2						0
#define LGT_IP_3						1
#define LGT_PORT						1234

LgtBuffer lgtRecvBuf, lgtSendBuf, lgtMsgBuf;
LgtContext lgtCtx;
list<LgtArchives> lgtDevList;
CSocketIf *pLgtSocket;
TTOnTimer g_tOnTimer[128];

uint8_t macMatrix[][8] = {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00},
};

#if 0
#include <list>
#include "CctMain.h"
#include "DbMgr.h"
#include "CctCfg.h"
#include "DbCfg.h"
#include "HisiReader.h"
#include "CctAppAdapter.h"

TCctPara tCctPara;
CCctReader* g_pCctReader[READER_NUM] = { NULL, NULL, NULL };

TCctDbPara tCctDbPara;
CCctAppAdapter *pAppAdapter = NULL;

TTOnTimer g_tOnTimer[128];

#endif

void LgtOpenComm()
{
	//lgtCtx.comm = new CComm;

}

void LgtInitLoginTimer()
{
	lgtCtx.loginTimer.bUse = 1;
	lgtCtx.loginTimer.bType = TIME_INTU_SEC;
	lgtCtx.loginTimer.dwIntV = 10;
	lgtCtx.loginTimer.dwID = APP_QUEUE_ID_DRV;
	lgtCtx.loginTimer.dwMsgId = LGT_MSG_ID_LOGIN_RESEND;
}

void StartLoginTimer()
{
	RegisterOnTimer(&lgtCtx.loginTimer);
}

void StopLoginTimer()
{
	DeleteOnTimer(&lgtCtx.loginTimer);
}

void LgtInitHeartTimer()
{
	lgtCtx.heartTimer.bUse = 1;
	lgtCtx.heartTimer.bType = TIME_INTU_SEC;
	lgtCtx.heartTimer.dwIntV = 10;
	lgtCtx.heartTimer.dwID = APP_QUEUE_ID_DRV;
	lgtCtx.heartTimer.dwMsgId = LGT_MSG_ID_HEART;
}

void StartHeartTimer()
{
	RegisterOnTimer(&lgtCtx.heartTimer);
}

void StopHeartTimer()
{
	DeleteOnTimer(&lgtCtx.heartTimer);
}

void LgtPrintArchives() {
	LgtArchives dev;
	for (list<LgtArchives>::iterator it = lgtDevList.begin(); it != lgtDevList.end(); it++) {
		dev = *it;
		TRACE("%02X:%02X:%02X:%02X:%02X:%02X onoff=%d bright=%d\r\n", dev.mac[0], dev.mac[1], dev.mac[2], dev.mac[3], dev.mac[4], dev.mac[5], dev.onoff, dev.bright);
	}
}

bool LgtAddArchives(LgtArchives *p) {
	LgtArchives dev;
	for (list<LgtArchives>::iterator it = lgtDevList.begin(); it != lgtDevList.end(); it++) {
		dev = *it;
		if ((memcmp(dev.mac, p->mac, 8) == 0)) {
			return false;
		}
	}
	lgtDevList.push_back(*p);
	return true;
}

bool LgtDelArchives(LgtArchives *p) {
	LgtArchives dev;
	for (list<LgtArchives>::iterator it = lgtDevList.begin(); it != lgtDevList.end(); it++) {
		dev = *it;
		if ((memcmp(dev.mac, p->mac, 8) == 0)) {
			lgtDevList.erase(it);
			return true;
		}
	}
	return false;
}

void LgtSetAllArchives(uint8_t onoff, uint8_t bright)
{
	for (list<LgtArchives>::iterator it = lgtDevList.begin(); it != lgtDevList.end(); it++) {
		it->onoff = onoff;
		it->bright = bright;

	}
}

bool LgtSetOneArchives(uint8_t *mac, uint8_t onoff, uint8_t bright)
{
	for (list<LgtArchives>::iterator it = lgtDevList.begin(); it != lgtDevList.end(); it++) {
		if (memcmp(mac, it->mac, 8) == 0)
		{
			it->onoff = onoff;
			it->bright = bright;
			return true;
		}
	}
	return false;
}

void LgtSystemInit() 
{
	uint16_t i;
	uint8_t ss[] = { 0x68,0x38,0x00,0xC3,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x02,0x01,0xB2,0x37,0x01,0x01,0x00,0x00,0x3C,0x0A,0x00,0x00,0x04,0x00,0x04,0x3C,0x78,0x56,0x00,0x34,0x12,0x00,0xF8,0x07,0x08,0x00,0x18,0x09,0x13,0x18,0x09,0x13,0x48,0x53,0x30,0x30,0x22,0x01,0x19,0x30,0x21,0x00,0x80,0xCA,0x16 };
	uint8_t tmp = 0x11;
	
	LgtMrsStruct lgt;
	/*
	TRACE("size = %d\r\n", sizeof(ss));
	int ret = LgtMrsUnpack(ss, sizeof(ss), &lgt);
	TRACE("ret=%d\r\n", ret);
	TRACE("%02x %d %d %d %d %d\r\n", lgt.ctrl, lgt.commFlag, lgt.id, lgt.afn, lgt.dt, lgt.dataLen);
	if (lgt.commFlag)
	TRACE("%02x:%02x:%02x:%02x:%02x:%02x  %02x:%02x:%02x:%02x:%02x:%02x\r\n", lgt.src[0], lgt.src[1], lgt.src[2], lgt.src[3], lgt.src[4], lgt.src[5], lgt.dest[0], lgt.dest[1], lgt.dest[2], lgt.dest[3], lgt.dest[4], lgt.dest[5]);
	for (i = 0; i < LGT_MAC_ADDRESS_LEN; i++)
	{
		lgtCtx.macAddr[i] = tmp;
		tmp += 0x11;
	}
	TRACEHEX("data", lgt.data, lgt.dataLen);
	*/

	lgt.afn = 0x14;
	lgt.dt = 2;
	lgt.dataLen = 0;
	lgt.commFlag = 0;
	lgt.ctrl = 0x03;
	lgt.id = 1;
	lgtSendBuf.len = LgtMrsPack(lgtSendBuf.buf, &lgt);
	TRACEHEX("data", lgtSendBuf.buf, lgtSendBuf.len);
	lgtCtx.servPort = LGT_PORT;
	lgtCtx.servIp[0] = LGT_IP_0;
	lgtCtx.servIp[1] = LGT_IP_1;
	lgtCtx.servIp[2] = LGT_IP_2;
	lgtCtx.servIp[3] = LGT_IP_3;
	lgtCtx.status = LGT_STATUS_IDLE;
	lgtCtx.id = 0;
	//lgtCtx.
	//InitQueue();
	//LgtInitLoginTimer();
	//LgtInitHeartTimer();
	/*
	LgtArchives dev;
	dev.bright = 0;
	dev.onoff = 1;
	memcpy(dev.mac, macMatrix[0], 6);
	LgtAddArchives(&dev);
	LgtPrintArchives();*/
}

uint16_t LgtLoginPack(uint8_t *str)
{
	PacketStruct pktStr;
	memcpy(pktStr.addr, lgtCtx.macAddr, LGT_MAC_ADDRESS_LEN);
	pktStr.fun = LGT_FUN_LOGIN_SERVER;
	pktStr.ctrl = 0;
	pktStr.dataLen = 0;
	return LgtPack(str, &pktStr);
}

uint16_t LgtHeartPack(uint8_t *str)
{
	PacketStruct pktStr;
	memcpy(pktStr.addr, lgtCtx.macAddr, LGT_MAC_ADDRESS_LEN);
	pktStr.fun = LGT_FUN_HEART;
	pktStr.ctrl = 0;
	pktStr.dataLen = 0;
	return LgtPack(str, &pktStr);
}

uint16_t LgtSyncArchivesPack(uint8_t *str)
{
	PacketStruct pktStr;
	memcpy(pktStr.addr, lgtCtx.macAddr, LGT_MAC_ADDRESS_LEN);
	pktStr.fun = LGT_FUN_SYNC_ARCHIVES;
	pktStr.ctrl = 0;
	pktStr.dataLen = 0;
	return LgtPack(str, &pktStr);
}

uint16_t LgtSetAllArchivesPack(uint8_t *str)
{
	PacketStruct pktStr;
	memcpy(pktStr.addr, lgtCtx.macAddr, LGT_MAC_ADDRESS_LEN);
	pktStr.fun = LGT_FUN_SET_ALL_LAMP;
	pktStr.ctrl = 0;
	pktStr.dataLen = 0;
	return LgtPack(str, &pktStr);
}

uint16_t LgtSetOneArchivesPack(uint8_t *str)
{
	PacketStruct pktStr;
	memcpy(pktStr.addr, lgtCtx.macAddr, LGT_MAC_ADDRESS_LEN);
	pktStr.fun = LGT_FUN_SET_ONE_LAMP;
	pktStr.ctrl = 0;
	pktStr.dataLen = 0;
	return LgtPack(str, &pktStr);
}

uint16_t LgtGetAllLampPack(uint8_t *str) {
	return 0;
}

void LgtRecvProc(PacketStruct *p)
{
	TRACE("fun=%04x\r\n", p->fun);
	switch (p->fun) {
	case LGT_FUN_LOGIN_SERVER:
		TRACE("server login success\r\n");
		StopLoginTimer();
		StartHeartTimer();
		break;
	case LGT_FUN_HEART:
		TRACE("heart\r\n");
		break;
	case LGT_FUN_SYNC_ARCHIVES: {
		TRACE("sync archives\r\n");
		LgtArchives dev;
		uint8_t cnt = p->data[0], offset = 1;
		for (uint8_t i = 0; i < p->dataLen; i++) {
			TRACE("%d=%02x\r\n", i, p->data[i]);
		}
		TRACE("cnt=%d\r\n", cnt); 
		dev.onoff = 0;
		dev.bright = 0;
		for (uint8_t i = 0; i < cnt; i++)
		{
			memcpy(dev.mac, &p->data[offset], 8);
			LgtAddArchives(&dev);
			offset += 8;
		}
		lgtSendBuf.len = LgtSyncArchivesPack(lgtSendBuf.buf);
		pLgtSocket->Send(lgtSendBuf.buf, lgtSendBuf.len);
		LgtPrintArchives();
	}
		break;
	case LGT_FUN_GET_ALL_ARCHIVES: {
		TRACE("get all archives\r\n");
	}
		break;
	case LGT_FUN_SET_ALL_LAMP:
		//���Ȳ���
		TRACE("set all lamp\r\n");
		if (p->dataLen != 2)
			return;
		TRACE("onoff=%d bright=%02x\r\n", p->data[0], p->data[1]);
		LgtSetAllArchives(p->data[0], p->data[1]);
		LgtPrintArchives();
		lgtSendBuf.len = LgtSetAllArchivesPack(lgtSendBuf.buf);
		pLgtSocket->Send(lgtSendBuf.buf, lgtSendBuf.len);
		break;
	case LGT_FUN_SET_ONE_LAMP:
		TRACE("set one lamp\r\n");
		if (p->dataLen != 10)
			return;
		LgtSetOneArchives(&p->data[0], p->data[8], p->data[9]);
		LgtPrintArchives();
		lgtSendBuf.len = LgtSetOneArchivesPack(lgtSendBuf.buf);
		pLgtSocket->Send(lgtSendBuf.buf, lgtSendBuf.len);
		break;
	case LGT_FUN_GET_ALL_LAMP:
		break;
	default:
		break;
	}
}

//*****************************************************************************
// ��������: ReMoteTask
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
// ��    ��: hewenbin
//*****************************************************************************
TTaskRet RemoteTask(void *pvArg)
{
	uint8_t cs;
	uint16_t validLen;
	int32_t readLen;
	pLgtSocket = new CSocketIf();
	pLgtSocket->ClientInit(lgtCtx.servIp, lgtCtx.servPort);
	if (pLgtSocket->IsConnected()) {
		//lgtMsgBuf.len = 1;
		//lgtMsgBuf.buf[0] = 10;
		//lgtCtx.status = LGT_STATUS_LOGINING;
		//AppSendMessage2Queue(APP_QUEUE_ID_DRV, LGT_LOGIN_SERVER, lgtMsgBuf.buf, lgtMsgBuf.len);
		lgtSendBuf.len = LgtLoginPack(lgtSendBuf.buf);
		pLgtSocket->Send(lgtSendBuf.buf, lgtSendBuf.len);
		StartLoginTimer();
	}
	while (1)
	{
		if (!pLgtSocket->IsConnected()) 
		{
			pLgtSocket->ClientInit(lgtCtx.servIp, lgtCtx.servPort);
			Sleep(500);
		}
		//��ȡ֡��ʼ����֡����
		readLen = pLgtSocket->Recv(lgtRecvBuf.buf, 3);
		if (readLen <= 0)
		{
			continue;
		}
		if (lgtRecvBuf.buf[0] != FRAME_START)
		{
			TRACE("first byte error\r\n");
			continue;
		}
		validLen = (uint16_t)(lgtRecvBuf.buf[1]) + ((uint16_t)(lgtRecvBuf.buf[2]) << 8);
		if (validLen >= 300)
		{
			TRACE("length is error\r\n");
			continue;
		}
		//��ȡ֡ʣ�ಿ�֣�����Ӧ���롢��ַ�򡢿����롢������У���롢֡��������
		readLen = pLgtSocket->Recv(&lgtRecvBuf.buf[3], validLen);
		if (readLen <= 0)
		{
			continue;
		}
		//����֡�ܳ���
		lgtRecvBuf.len = readLen + 3;
		cs = LgtSum(lgtRecvBuf.buf, lgtRecvBuf.len - 2);
		PacketStruct packStr;
		int res = LgtUnpack(lgtRecvBuf.buf, lgtRecvBuf.len, &packStr);
		if (res < 0)
		{
			TRACE("unpack error %d\r\n", res);
			continue;
		}
		//��������֡����
		for (int i = 0; i < lgtRecvBuf.len; i++) {
			TRACE("%d=%02x\r\n", i, lgtRecvBuf.buf[i]);
		}
		LgtRecvProc(&packStr);
	}
}

TTaskRet KeepAliveTask(void *pvArg)
{
	uint8_t buf[384];
	uint32_t len;
	TTime dt;
	APP_MSG_ID_ENUM dwMsgId;
	while (1) 
	{
		if (pLgtSocket == NULL)
		{
			Sleep(500);
			continue;
		}
		if (!pLgtSocket->IsConnected())
		{
			Sleep(500);
			continue;
		}
		int32_t ret = AppRcvMessage(APP_QUEUE_ID_DRV, &dwMsgId, buf, &len, 0xffffffff);
		if (ret == 0)
		{
			switch (dwMsgId)
			{
			case LGT_MSG_ID_LOGIN_RESEND:
				TRACE("LGT_LOGIN_SERVER_RESEND\n");
				lgtSendBuf.len = LgtLoginPack(lgtSendBuf.buf);
				pLgtSocket->Send(lgtSendBuf.buf, lgtSendBuf.len);
				break;
			case LGT_MSG_ID_HEART:
				TRACE("LGT_MSG_ID_HEART\r\n");
				lgtSendBuf.len = LgtHeartPack(lgtSendBuf.buf);
				pLgtSocket->Send(lgtSendBuf.buf, lgtSendBuf.len);
				break;
			}
		}
		GetTime(&dt);
		TRACE("%d %d %d\r\n", dt.bHour, dt.bMinute, dt.bSecond);
		Sleep(1000);
	}
}

//*****************************************************************************
// ��������: OnTimerTask
// ��������: ��ʱ������
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
TTaskRet OnTimerTask(void *pvArg)
{
	TRACE("OnTimerTask::Run().\n");
	memset(g_tOnTimer, 0, sizeof(g_tOnTimer));
	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwRxLen = 0;
	unsigned int dwClick = 0;
	unsigned char *pRxBuf = new unsigned char[2048];
	if (pRxBuf == NULL) 
		return 0;
	TTOnTimer *ptOnTimer = NULL;
	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 10;
	tOnTimer.dwID = APP_QUEUE_ID_TIM;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);
	for (;;)
	{
		if (0 == AppRcvMessage(APP_QUEUE_ID_TIM, &dwMsgId, pRxBuf, &dwRxLen, 0))
		{
			ptOnTimer = (TTOnTimer *)pRxBuf;
			switch (dwMsgId)
			{
				//��Ӷ�ʱ��
			case APP_MSG_ID_REG_ONTIMER:
				AddOnTimer((TTOnTimer *)pRxBuf);
				break;
				//ɾ����ʱ��
			case APP_MSG_ID_DEL_ONTIMER:
				DeleteOnTimer(ptOnTimer);
				break;
			case APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE:
				OnTimerTaskKeepAlive();
				break;
			default:
				TRACE("OnTimerTask MsgId err.\n");
				break;
			}
		}
		CheckTimeOver();
		//�����ʱ�����ȿ��ܲ����� ���´�ӡ�����ڵ���10ms��ʱ������
		//TRACE("OnTimerTask %d\n", GetTick());
		//�����ӳ���Ҫ��������֤10MS��ʱ���ڵ�ǰ�߳����ȼ��µľ�׼��
		Sleep(8);
	}
	return 0;
}



//*****************************************************************************
// ��������: AppMainTask
// ��������: ������
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
TTaskRet AppMainTask(void *pvArg)
{
	LgtSystemInit();

#if 0
	SysInit();

	//���ݿ����
	NewTask(CctDbMgrTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//ͨ�Ž����߳�
	NewTask(MsgDrvTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//ͨ�ŷ����߳�
	NewTask(MsgSendTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//PLC����
	NewTask(CctReaderTask, NULL, 0, THREAD_PRIORITY_NORMAL);


	NewTask(CctReciveTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//��ʱ������
	NewTask(OnTimerTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//�̼߳��
	NewTask(MonitorTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//Զ�̵��Լ��
	NewTask(ReMoteTask, NULL, 0, THREAD_PRIORITY_NORMAL);

	//��ҵ�����߳�
	Run(pvArg);
#endif
	NewTask(RemoteTask, NULL, 0, THREAD_PRIORITY_NORMAL);
	NewTask(KeepAliveTask, NULL, 0, THREAD_PRIORITY_NORMAL);
	NewTask(OnTimerTask, NULL, 0, THREAD_PRIORITY_NORMAL);
	//NewTask(OnTimerTask, NULL, 0, THREAD_PRIORITY_NORMAL);
	while (1)
	{
		
	}
	/*
	APP_MSG_ID_ENUM dwMsgId;
	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_MAIN;
	tOnTimer.dwMsgId = APP_MSG_ID_TIMER_HEART_BEAT;
	RegisterOnTimer(&tOnTimer);
	int32_t t;
	uint32_t len;
	AddOnTimer(&tOnTimer);
	for (;;)
	{
		TRACE("##############################################\r\n");
		t == AppRcvMessage(APP_QUEUE_ID_MAIN, &dwMsgId, lgtRecvBuf.buf, &len, 0xffffffff);
		TRACE("..........................=%d\r\n", dwMsgId);
		
		//if (t == AppRcvMessage(APP_QUEUE_ID_MAIN, &dwMsgId, lgtRecvBuf.buf, &lgtRecvBuf.len, 0xffffffff))
		//if (t == 0)
		//{
			//AppMainMsgProc(dwMsgId, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
			TRACE("hello\r\n");
		//}
		//TRACE("t=%d\r\n", t);
		Sleep(100);
	}
	*/
	return 0;
}

#if 0
//*****************************************************************************
// ��������: Run
// ��������: ҵ����Ϣ�ַ�
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
void Run(void *pvArg)
{
	TRACE("AppCctMainTask::Run().\n");

	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwRxLen = APP_CCT_BUF_SIZE;
	unsigned int dwTxLen = APP_CCT_BUF_SIZE;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return;
	memset(pRxBuf, 0, APP_CCT_BUF_SIZE);

	unsigned char *pTxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pTxBuf == NULL)return;
	memset(pTxBuf, 0, APP_CCT_BUF_SIZE);

	//ע�ᶨʱ��-���ڷ�������֡
	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 280;
	tOnTimer.dwID = APP_QUEUE_ID_MAIN;
	tOnTimer.dwMsgId = APP_MSG_ID_TIMER_HEART_BEAT;
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_MAIN;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_MAIN_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);

	for (;;)
	{
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_MAIN, &dwMsgId, pRxBuf, &dwRxLen, 0xffffffff))
		{
			AppMainMsgProc(dwMsgId, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		}

		//Sleep(10);
	}

	return;
}

//*****************************************************************************
// ��������: CctMainTaskKeepAlive
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
void CctMainTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_MAIN_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}


//*****************************************************************************
// ��������: AppMainMsgProc
// ��������: ҵ����������Ϣ�ַ�
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
signed int AppMainMsgProc(unsigned int dwMsgId, unsigned char *pRxBuf, unsigned int dwRxLen, unsigned char *pTxBuf, unsigned int dwTxLen)
{	
	
	if ((NULL == pRxBuf) || (0 == dwRxLen) || (pAppAdapter == NULL))
	{
		return 0;
	}

	switch (dwMsgId)
	{
	case APP_MSG_ID_SVR_REQ:
		pAppAdapter->DispatchPackage(pRxBuf, pTxBuf);
		break;

	case APP_MSG_ID_SVR_REP_645_DATA:
		pAppAdapter->AdapterDoPackage(ADAPTER_CCT_APP_FN4, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		break;

	case APP_MSG_ID_SVR_REP_EVENT_DATA:
		pAppAdapter->AdapterDoPackage(ADAPTER_CCT_APP_FN5, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		break;

	case APP_MSG_ID_SVR_REP_TOP_DATA:
		pAppAdapter->AdapterDoPackage(ADAPTER_CCT_APP_FN6, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		break;
	case APP_MSG_ID_TIMER_HEART_BEAT:
		pAppAdapter->CctAppHeartBeat(ADAPTER_CCT_APP_FN1, pRxBuf, dwRxLen, pTxBuf, dwTxLen);
		break;
	case APP_MSG_ID_CCT_MAINNODE:
		pAppAdapter->AdapterDoSaveMainNode(pRxBuf, dwRxLen);
		break;

	case APP_MSG_ID_CCT_MAIN_TASK_ALIVE:
		CctMainTaskKeepAlive();
		break;
		
	default:

		break;
	}

	return 0;

}


#define FSM_CON_STATUS_DISCON			0
#define FSM_CON_STATUS_IF_CON			1
#define FSM_CON_STATUS_TRAN				2

//*****************************************************************************
// ��������: MsgDrvTask
// ��������: ͨ���̡߳����շ�������������������
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
TTaskRet MsgDrvTask(void *pvArg)
{
	TRACE("MsgDrvTask::Run().\n");

	unsigned int dwTxLen = 0;
	unsigned int dwRxLen = 0;
	unsigned char bConStatus = 0;
	unsigned int dwClick = 0;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return 0;
	unsigned char *pTxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pTxBuf == NULL)return 0;
	memset(pRxBuf, 0, APP_CCT_BUF_SIZE);
	memset(pTxBuf, 0, APP_CCT_BUF_SIZE);

	APP_MSG_ID_ENUM tMsgId;
	unsigned int dwMsgRxLen = 0;
	unsigned char *pMsgRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return 0;
	memset(pMsgRxBuf, 0, APP_CCT_BUF_SIZE);

	//ҵ��������
	pAppAdapter = new CCctAppAdapter();
	if (pAppAdapter == NULL) return 0;
	pAppAdapter->Init();
	bConStatus = FSM_CON_STATUS_TRAN;

	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_10MSEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_DRVRCV;
	tOnTimer.dwMsgId = APP_MSG_ID_DRV_10MS;
	//10ms��ʱ��
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_DRVRCV;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);

	for (;;)
	{

		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_DRVRCV, &tMsgId, pMsgRxBuf, &dwMsgRxLen, 0xffffffff))
		{

			switch(tMsgId)
			{
			case APP_MSG_ID_DRV_10MS:
#if 1
				if (!pAppAdapter->IsAdapterConnected())
				{
					//���ߵȴ�30S������ �ӿڡ�������
					//Sleep(5000);
					if (dwClick + 5 < GetClick())
					{
						dwClick = GetClick();
						pAppAdapter->ConnectAdapter();
						continue;
					}

				}
				else
				{
					//���շ�������������
					if (true == pAppAdapter->RcvFrm(pRxBuf, &dwRxLen))
					{
						TRACEHEX("==>CSocketIf::SocketRecv", pRxBuf, dwRxLen);
						AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REQ, pRxBuf, dwRxLen);
					}
			
#else
			
					if (CheckSvrLinkFsm())
					{
						//���շ�������������
						if (true == pAppAdapter->RcvFrm(pRxBuf, &dwRxLen))
						{
							TRACEHEX("==>SocketRcv", pRxBuf, dwRxLen);
							AppSendMessage2Queue(APP_QUEUE_ID_MAIN, APP_MSG_ID_SVR_REQ, pRxBuf, dwRxLen);
						}
					}
#endif	
				}

				break;

			case APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE:
				MsgDrvTaskKeepAlive();
				break;

			default:
				TRACE("MsgDrvTask MsgId err !.\n");
				break;
				
			}

		//Sleep(10);
		
		}
		
	}

	return 0;
}

//*****************************************************************************
// ��������: MsgDrvTaskKeepAlive
// ��������: �����ֹ�����ļ��
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
void MsgDrvTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

#if 0
//*****************************************************************************
// ��������: CheckSvrLinkFsm
// ��������: ͨ����·״̬ά��
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
bool CheckSvrLinkFsm()
{
	static unsigned char m_pIfReCon = 0;
	static unsigned char m_pAdapterReCon = 0;
	unsigned char bConStatus = FSM_CON_STATUS_DISCON;

	for (;;)
	{
		switch(bConStatus)
		{
		case FSM_CON_STATUS_DISCON:
			if (!pAppAdapter->m_pIf->IsConnected())
			{
				pAppAdapter->m_pIf->SocketReConnect();
				if (pAppAdapter->m_pIf->IsConnected())
				{
					bConStatus = FSM_CON_STATUS_IF_CON;
					m_pIfReCon = 0;
				}
				else
				{
					m_pIfReCon++;
					if (m_pIfReCon > 3)
					{
						m_pIfReCon = 0;
						bConStatus = FSM_CON_STATUS_DISCON;
						Sleep(300000);
					}
					
					Sleep(5000);
					continue;
				}
			}
			else
			{
				bConStatus = FSM_CON_STATUS_IF_CON;
			}
			break;
			
		case FSM_CON_STATUS_IF_CON:
			if (!pAppAdapter->IsAdapterConnected())
			{
				pAppAdapter->ConnectAdapter();
				if (pAppAdapter->IsAdapterConnected())
				{
					bConStatus = FSM_CON_STATUS_TRAN;
					m_pAdapterReCon = 0;
				}
				else
				{
					m_pAdapterReCon++;
					if (m_pAdapterReCon > 3)
					{
						m_pAdapterReCon = 0;
						bConStatus = FSM_CON_STATUS_TRAN;
						Sleep(300000);
					}
					
					Sleep(5000);
					continue;
				}
			}
			else
			{
				bConStatus = FSM_CON_STATUS_TRAN;
			}
			break;
			
		case FSM_CON_STATUS_TRAN:
			return true;
			break;
			
		default:
		
			break;
		
		}
	}


}
#endif

//*****************************************************************************
// ��������: MsgSnedTask
// ��������: ͨ���̡߳��������ݵ�������
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
TTaskRet MsgSendTask(void *pvArg)
{
	TRACE("MsgSendTask::Run().\n");

	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwTxLen = 0;

	unsigned char *pTxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pTxBuf == NULL)return 0;

	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_DRV;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);


	for (;;)
	{
		if (NULL != pAppAdapter)
		{
			if (!pAppAdapter->IsAdapterConnected())
			{
				Sleep(1000);
				continue;
			}
		}
		else
		{
			Sleep(1000);
			continue;
		}
	
		//������վ������ ���ߺ������ڶ�������
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_DRV, &dwMsgId, pTxBuf, &dwTxLen, 0xffffffff))
		{
			switch (dwMsgId)
			{
			case APP_MSG_ID_SVR_REP:
				TRACEHEX("<==CSocketIf::SocketSend", pTxBuf, dwTxLen);
				pAppAdapter->Send(pTxBuf, dwTxLen);
				break;

			case APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE:
				MsgSendTaskKeepAlive();
				break;

			default:
				TRACE("MsgSendTask MsgId err !.\n");
				break;
			}
		}

		//Sleep(10);
	}

}


//*****************************************************************************
// ��������: MsgSendTaskKeepAlive
// ��������: �����ֹ�����ļ��
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
void MsgSendTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

//*****************************************************************************
// ��������: CctDbMgrTask
// ��������: ���ݿ����
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
TTaskRet CctDbMgrTask(void *pvArg)
{
	if (g_pDbMgr == NULL)
	{
		TRACE("DbgInit Fail.\n");
		return 0;
	}

	g_pDbMgr->Run(pvArg);

	return 0;
}

//*****************************************************************************
// ��������: CctReaderTask
// ��������: PLC��������
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
TTaskRet CctReaderTask(void *pvArg)
{
	if (g_pCctReader[CCTPLC_READER] != NULL)
	{
		//������
		g_pCctReader[CCTPLC_READER]->Run(pvArg);
	}

	return 0;
}

//*****************************************************************************
// ��������: CctReciveTask
// ��������: PLC�����������ݽ���
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
TTaskRet CctReciveTask(void *pvArg)
{
	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_10MSEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_CCTRCV;
	tOnTimer.dwMsgId = APP_MSG_ID_DRV_10MS;
	//10ms��ʱ��
	RegisterOnTimer(&tOnTimer);

	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_CCTRCV;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);


	if (g_pCctReader[CCTPLC_READER] != NULL)
	{
#if defined(PRODUCT_CFG_PRODUCT_TYPE_MRS)
		g_pCctReader[CCTPLC_READER]->DoCctRcvFrm();
#endif

#if defined(PRODUCT_CFG_PRODUCT_TYPE_CSG)
		g_pCctReader[CCTPLC_READER]->DoCsgCctRcvFrm();
#endif

	}

	return 0;
}


//*****************************************************************************
// ��������: OnTimerTask
// ��������: ��ʱ������
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
TTaskRet OnTimerTask(void *pvArg)
{
	TRACE("OnTimerTask::Run().\n");
	memset(g_tOnTimer, 0, sizeof(g_tOnTimer));

	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwRxLen = 0;
	unsigned int dwClick = 0;

	unsigned char *pRxBuf = new unsigned char[APP_CCT_BUF_SIZE];
	if (pRxBuf == NULL)return 0;

	TTOnTimer *ptOnTimer = NULL;

	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = TASK_KEEP_ALIVE_CHECK_TIME;
	tOnTimer.dwID = APP_QUEUE_ID_TIM;
	tOnTimer.dwMsgId = APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE;
	//�̵߳��������
	RegisterOnTimer(&tOnTimer);

	for (;;)
	{	
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_TIM, &dwMsgId, pRxBuf, &dwRxLen, 0))
		{
			ptOnTimer = (TTOnTimer *)pRxBuf;

			switch (dwMsgId)
			{
				//��Ӷ�ʱ��
			case APP_MSG_ID_REG_ONTIMER:
				AddOnTimer((TTOnTimer *)pRxBuf);
				break;

				//ɾ����ʱ��
			case APP_MSG_ID_DEL_ONTIMER:
				DeleteOnTimer(ptOnTimer);
				break;

			case APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE:
				OnTimerTaskKeepAlive();
				break;

			default:
				TRACE("OnTimerTask MsgId err.\n");
				break;
			}
		}

		CheckTimeOver();

		//�����ʱ�����ȿ��ܲ����� ���´�ӡ�����ڵ���10ms��ʱ������
		//TRACE("OnTimerTask %d\n", GetTick());

		//�����ӳ���Ҫ��������֤10MS��ʱ���ڵ�ǰ�߳����ȼ��µľ�׼��
		Sleep(8);
	}

	return 0;
}

//*****************************************************************************
// ��������: MonitorTask
// ��������: �̼߳��
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
TTaskRet MonitorTask(void *pvArg)
{
	TRACE("MonitorTask::Run().\n");
	unsigned int dwClick = 0;
	APP_MSG_ID_ENUM dwMsgId;
	unsigned int dwSize = 0;

	TTOnTimer tOnTimer;
	tOnTimer.bUse = 1;
	tOnTimer.bType = TIME_INTU_SEC;
	tOnTimer.dwIntV = 1;
	tOnTimer.dwID = APP_QUEUE_ID_MINITOR;
	tOnTimer.dwMsgId = APP_MSG_ID_SEC_ONTIMER;
	RegisterOnTimer(&tOnTimer);	
	
	for (;;)
	{	
		if (CCT_ERR_SUCCESS == AppRcvMessage(APP_QUEUE_ID_MINITOR, (APP_MSG_ID_ENUM *)&dwMsgId, (unsigned char *)&dwClick, &dwSize, 0xffffffff))
		{
			continue;
			//TRACE("dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
			
			switch(dwMsgId)
			{
			case APP_MSG_ID_CCT_DBMGR_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_DBMGR_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;

			case APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;
					
			case APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;
				
			case APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;
				
			case APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;

			case APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;

			case APP_MSG_ID_CCT_MAIN_TASK_ALIVE:
				TRACE("APP_MSG_ID_CCT_MAIN_TASK_ALIVE dwMsgId %d, dwClick %d.\n", dwMsgId, dwClick);
				break;

			case APP_MSG_ID_SEC_ONTIMER:
				TRACE("MonitorTask APP_MSG_ID_SEC_ONTIMER.\n");
				break;
				
			default:
				TRACE("MonitorTask MsgId err.\n");
				break;
			}
		}
	
		//TRACE("MonitorTask.\n");
		//Sleep(10);
	}

	return 0;
}


//*****************************************************************************
// ��������: SysInit
// ��������: ϵͳʹ�������Դ��ʼ��
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
void SysInit()
{
	InitDebug();

	CCT_APP_INFO tAppVersion;
	memcpy(&tAppVersion, &g_AppVersion, sizeof(CCT_APP_INFO));

	TRACE("/**************************************************/\n");
	TRACE("/***        LQBS_CCT_APP@www.richnano.cn        ***/\n");
	TRACE("/***        LQBS INTELLIGENT LTE MODULE         ***/\n");
#if defined(PRODUCT_CFG_PRODUCT_TYPE_MRS)	
	TRACE("/***        PRODUCT_CFG_PRODUCT_TYPE_MRS        ***/\n");
#endif
#if defined(PRODUCT_CFG_PRODUCT_TYPE_CSG)	
	TRACE("/***        PRODUCT_CFG_PRODUCT_TYPE_CSG        ***/\n");
#endif	
	TRACE("/***        APPVERSION: %d-%d-%d %d:%d:%d        ***/\n", 
							tAppVersion.tTime.wYear, 
							tAppVersion.tTime.bMonth,
							tAppVersion.tTime.bDay, 
							tAppVersion.tTime.bHour, 
							tAppVersion.tTime.bMinute, 
							tAppVersion.tTime.bSecond);
	TRACE("/**************************************************/\n\n");

	InitVar();

	//��ʼ����Ϣ����
	InitQueue();

	InitDb();

	InitCctReader();
	
	//�̼߳��
	InitTaskMonitor();	

}

//*****************************************************************************
// ��������: InitVar
// ��������: ȫ�ֱ�����ʼ��
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
void InitVar()
{
	;
}

//*****************************************************************************
// ��������: InitDb
// ��������: ���ݿ��ʼ��
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
void InitDb()
{
	g_pDbMgr = new CDbMgr();

	if (g_pDbMgr == NULL)
	{
		TRACE("DbgInit Fail.\n");
		return;
	}

	GetDbPara(&tCctDbPara);
	g_pDbMgr->Init(&tCctDbPara);
}



//*****************************************************************************
// ��������: InitCctReader
// ��������: ����������ʵ��
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
bool InitCctReader()
{
	//������	
	//PLC������ʵ��
	CHisiReader *pCctReader = new CHisiReader();
	if (pCctReader == NULL)
	{
		TRACE("InitCctReader: Fail to new CHisiReader!\r\n");
		return false;
	}

	//װ�ز���
	LoadCctPara(ROUTER_LINK_HISI, &tCctPara);
	pCctReader->Init(&tCctPara);

	g_pCctReader[CCTPLC_READER] = (CCctReader*)pCctReader;

	return true;
}

//*****************************************************************************
// ��������: DbgInit
// ��������: ��ʼ�������������
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
void InitDebug()
{
	;
}

//*****************************************************************************
// ��������: InitTaskMonitor
// ��������: ����������
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
void InitTaskMonitor()
{

	;
}
#endif
