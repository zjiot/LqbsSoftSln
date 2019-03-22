//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : MsgQueue.h
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************

#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include "arch.h"


//消息ID-消息的出入口
//如果多服务器连接 需要多开MsgDrvTask
//多开一个服务器连接需要增加一下四个类型消息
//目前只开了一个服务器连接
typedef enum
{
	//服务器请求
	APP_MSG_ID_SVR_REQ = 0,
	APP_MSG_ID_SVR_REP,

	//数据库请求
	APP_MSG_ID_DB_REQ,
	APP_MSG_ID_DB_REP,	

	//主动路由请求抄读节点数据
	APP_MSG_ID_CCT_REQ,
	APP_MSG_ID_CCT_CON,

	//路由中的拓扑信息
	APP_MSG_ID_CCT_TOP_REQ,
	APP_MSG_ID_SVR_REP_TOP_DATA,
	APP_MSG_ID_TIMER_HEART_BEAT,

	//645抄表数据返回
	APP_MSG_ID_SVR_REP_645_DATA,

	//事件上报
	APP_MSG_ID_SVR_REP_EVENT_DATA,

	//定时器注册/删除消息
	APP_MSG_ID_REG_ONTIMER,
	APP_MSG_ID_DEL_ONTIMER,

	//秒定时器消息ID
	APP_MSG_ID_SEC_ONTIMER,
	APP_MSG_ID_10MS_ONTIMER,

	//向LTE通信线程队列发起抄读总表请求(通过配变终端-376.1协议)
	APP_MSG_ID_LTE_REQ,
	APP_MSG_ID_LTE_REP,

	APP_MSG_ID_DRV_10MS,
	APP_MSG_ID_CCT_MAINNODE,

	//以下MsgID专用于线程监控
	/////////////////////////////////
	APP_MSG_ID_CCT_DBMGR_TASK_ALIVE,
	APP_MSG_ID_CCT_MSGDRV_TASK_ALIVE,
	APP_MSG_ID_CCT_MSGSEND_TASK_ALIVE,
	APP_MSG_ID_CCT_CCTREADER_TASK_ALIVE,
	APP_MSG_ID_CCT_CCTRECV_TASK_ALIVE,
	APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE,
	APP_MSG_ID_CCT_MAIN_TASK_ALIVE,

	//串口死机检测
	APP_MSG_ID_CCT_CCTREADER_TASK_UARTPULSE,
	//APP_MSG_ID_CCT_MONITOR_TASK,
	/////////////////////////////////

	// TODO ... 
	LGT_MSG_ID_LOGIN_RESEND,
	LGT_MSG_ID_HEART,
	APP_MSG_ID_MAX
}APP_MSG_ID_ENUM;


//队列ID
//如果多服务器连接 需要多开多个MsgDrvTask
//同时增加另外队列的ID标识APP_MAIN_QUEUE_DRV1、2、3....
typedef enum
{
	APP_QUEUE_ID_MAIN = 0,		//主线程队列
	APP_QUEUE_ID_DRV,			//通信线程队列
	APP_QUEUE_ID_DRVRCV,		//通信线程队列-接收
	APP_QUEUE_ID_DB,			//数据库管理线程队列
	APP_QUEUE_ID_CCT,			//PLC抄表线程队列
	APP_QUEUE_ID_CCTRCV,		//PLC抄表数据接收线程队列
	APP_QUEUE_ID_TIM,			//定时器线程队列
	APP_QUEUE_ID_LTE,			//LTE模块通信线程维护队列
	APP_QUEUE_ID_MINITOR,		//线程监控线程队列
    // TODO ... 
	
	APP_QUEUE_ID_MAX		//队列编号结束，放在末尾
} APP_QUEUE_ID_ENUM;


bool InitQueue();
const char *GetQueueName(signed int dwQueueId);
signed int GetQueueHandle(APP_QUEUE_ID_ENUM QueueId);
bool QueueName2Id(const char *pszName, signed int *pdwQueueId);
signed int AppMainMsgProc(unsigned int dwMsgId, unsigned char *pRxBuf, unsigned int dwRxLen, unsigned char *pTxBuf, unsigned int dwTxLen);
signed int AppSendMessage2Queue(APP_QUEUE_ID_ENUM dwQueId, APP_MSG_ID_ENUM dwMsgId, unsigned char *pBuf, unsigned int dwSize);
signed int AppRcvMessage(APP_QUEUE_ID_ENUM dwQueId, APP_MSG_ID_ENUM *pMsgId, unsigned char *pBuf, unsigned int *pSize, unsigned int dwTimeOut);


#endif
