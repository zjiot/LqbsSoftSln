//*****************************************************************************
//
//                  版权所有 (C), 2018-2028, 联强邦盛
//
//  文 件 名   : Cct.cpp
//  版 本 号   : V1.0
//  作    者   : sUesTcxF
//  生成日期   : 2018-10-25
//  功能描述   : 操作系统差异相关的接口定义
//
//  函数列表   :
//  修改历史   :
//  1.日    期 : 2018-10-25
//    作    者 : sUesTcxF
//    修改内容 : 创建文件
//
//*****************************************************************************
#include<pthread.h>

#include "arch.h"
#include "Cct.h"
#include "CctMain.h"

//extern TTaskRet AppMainTask(void *pArg);

int main(int argc,char **argv)
{
	//起业务线程
	//NewTask(AppMainTask, NULL, 0, THREAD_PRIORITY_NORMAL);
	AppMainTask(NULL);
	return 0;
}

