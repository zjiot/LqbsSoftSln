//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : OnTimer.cpp
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

#include "OnTimer.h"


LIST_TIMERINFO m_Ontimerlist;

//*****************************************************************************
// ��������: CheckTimeOver
// ��������: ��ⳬʱ
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
					//10ms��ʱ����������
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
		
				//��ʱʱ�䵽
				if (dwIntCurSecond >= iter->dwStart + iter->dwIntV*dwIntVSecond)
				{
					//ˢ���¸����ڵĿ�ʼʱ��
					iter->dwStart = dwIntCurSecond;
					AppSendMessage2Queue((APP_QUEUE_ID_ENUM)iter->dwID, (APP_MSG_ID_ENUM)iter->dwMsgId, (unsigned char *)&(*iter), sizeof(TTOnTimer));
				}
			}
		}
	}

}


//*****************************************************************************
// ��������: RegesiterTimer
// ��������: ע�ᶨʱ��
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
signed int RegisterOnTimer(TTOnTimer *pOnTimer)
{
	return AppSendMessage2Queue(APP_QUEUE_ID_TIM, APP_MSG_ID_REG_ONTIMER, (unsigned char *)pOnTimer, sizeof(TTOnTimer));

}

//*****************************************************************************
// ��������: AddOnTimer
// ��������: ��ʱ���б�������Ӷ�ʱ��
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
signed int AddOnTimer(TTOnTimer *ptOnTimer)
{
	if (NULL == ptOnTimer) return (-1);
	ptOnTimer->bUse = 1;
	ptOnTimer->dwStart = GetClick();
	m_Ontimerlist.push_back(*ptOnTimer);
	return 0;
}

//*****************************************************************************
// ��������: DeleteOnTimer
// ��������: ɾ����ʱ��
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
// ��������: OnTimerTaskKeepAlive
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
void OnTimerTaskKeepAlive()
{
	unsigned int dwClick = GetClick();
	AppSendMessage2Queue(APP_QUEUE_ID_MINITOR, APP_MSG_ID_CCT_ONTIMER_TASK_ALIVE, (unsigned char *)&dwClick, sizeof(unsigned int));
}

