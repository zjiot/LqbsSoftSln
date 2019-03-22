//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : OnTimer.cpp
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

#include "OnTimer.h"


LIST_TIMERINFO m_Ontimerlist;

//*****************************************************************************
// 函数名称: CheckTimeOver
// 功能描述: 检测超时
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
void CheckTimeOver()
{
	unsigned int dwClick = 0;
	unsigned dwIntVSecond = 0;
	unsigned dwIntCurSecond = 0;
	LIST_TIMERINFO_ITER iter;

	if (!m_Ontimerlist.empty())
	{		
		dwIntCurSecond = GetClick();
		for (iter = m_Ontimerlist.begin(); iter != m_Ontimerlist.end(); iter++)
		{
			if (iter->bUse)
			{
				switch (iter->bType)
				{
				case TIME_INTU_10MSEC:
					//10ms定时器单独处理
					AppSendMessage2Queue((APP_QUEUE_ID_ENUM)iter->dwID, (APP_MSG_ID_ENUM)iter->dwMsgId, (unsigned char *)&(*iter), sizeof(TTOnTimer));
					continue;
					break;
				
				case TIME_INTU_SEC:
					dwIntVSecond = 1;
					break;
		
				case TIME_INTU_MIN:
					dwIntVSecond = 60;
					break;
		
				case TIME_INTU_HOUR:
					dwIntVSecond = 3600;
					break;
		
				case TIME_INTU_DAY:
					dwIntVSecond = 86400;
					break;
				default:
		
					TRACE("CheckTimeOver default bType.\n");
					dwIntVSecond = 1;
					break;
				}
		
				//超时时间到
				if (dwIntCurSecond >= iter->dwStart + iter->dwIntV*dwIntVSecond)
				{
					//刷新下个周期的开始时间
					iter->dwStart = dwIntCurSecond;
					AppSendMessage2Queue((APP_QUEUE_ID_ENUM)iter->dwID, (APP_MSG_ID_ENUM)iter->dwMsgId, (unsigned char *)&(*iter), sizeof(TTOnTimer));
				}
			}
		}
	}

}


//*****************************************************************************
// 函数名称: RegesiterTimer
// 功能描述: 注册定时器
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
signed int RegisterOnTimer(TTOnTimer *pOnTimer)
{
	return AppSendMessage2Queue(APP_QUEUE_ID_TIM, APP_MSG_ID_REG_ONTIMER, (unsigned char *)pOnTimer, sizeof(TTOnTimer));

}

//*****************************************************************************
// 函数名称: AddOnTimer
// 功能描述: 向定时器列表里面添加定时器
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
signed int AddOnTimer(TTOnTimer *ptOnTimer)
{
	if (NULL == ptOnTimer) return (-1);
	ptOnTimer->bUse = 1;
	ptOnTimer->dwStart = GetClick();
	m_Ontimerlist.push_back(*ptOnTimer);
	return 0;
}

//*****************************************************************************
// 函数名称: DeleteOnTimer
// 功能描述: 删除定时器
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
signed int DeleteOnTimer(TTOnTimer *pOnTimer)
{
	if (pOnTimer == NULL) return (-1);

	LIST_TIMERINFO_ITER iter;
	for (iter = m_Ontimerlist.begin(); iter != m_Ontimerlist.end(); iter++)
	{
		if (iter->bUse == 1 &&
			iter->bType == pOnTimer->bType &&
			iter->dwID == pOnTimer->dwID &&
			iter->dwMsgId == pOnTimer->dwMsgId)
		{
			m_Ontimerlist.erase(iter);
			return 0;
		}
	}

	return (-1);
}

//*****************************************************************************
// 函数名称: OnTimerTaskKeepAlive
// 功能描述: 软件防止死机的检测
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
void OnTimerTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

