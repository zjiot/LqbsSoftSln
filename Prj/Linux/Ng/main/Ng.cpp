//*****************************************************************************
//
//                  ��Ȩ���� (C), 2018-2028, ��ǿ��ʢ
//
//  �� �� ��   : Cct.cpp
//  �� �� ��   : V1.0
//  ��    ��   : sUesTcxF
//  ��������   : 2018-10-25
//  ��������   : ����ϵͳ������صĽӿڶ���
//
//  �����б�   :
//  �޸���ʷ   :
//  1.��    �� : 2018-10-25
//    ��    �� : sUesTcxF
//    �޸����� : �����ļ�
//
//*****************************************************************************
#include<pthread.h>

#include "arch.h"
#include "Cct.h"
#include "CctMain.h"

//extern TTaskRet AppMainTask(void *pArg);

int main(int argc,char **argv)
{
	//��ҵ���߳�
	//NewTask(AppMainTask, NULL, 0, THREAD_PRIORITY_NORMAL);
	AppMainTask(NULL);
	return 0;
}

