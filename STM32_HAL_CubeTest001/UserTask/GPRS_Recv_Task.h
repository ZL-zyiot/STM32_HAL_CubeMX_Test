/***********************************************************************************
 * 文 件 名   : GPRS_Recv_Task.h
 * 负 责 人   : Z&L
 * 创建日期   : 2021年10月8日
 * 文件描述   : GPRS接收任务头文件
 * 版权说明   : Copyright (c)
 * 其    他     : 
 * 修改日志   : 
***********************************************************************************/



#ifndef __GPRS_RECV_TASK_H__
#define __GPRS_RECV_TASK_H__

#include "main.h"

typedef struct
{
	uint8_t *pdata;
	uint32_t *num;
}GPRS_Recv_Frame_t;


void initGPRS_Recv_Task(void);

void GPRS_Recv_Process(void);


#endif

