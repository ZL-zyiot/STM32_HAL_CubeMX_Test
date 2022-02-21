/***********************************************************************************
 * 文 件 名   : GPRS_Send_Task.h
 * 负 责 人   : Z&L
 * 创建日期   : 2021年10月8日
 * 文件描述   : GPRS发送任务头文件
 * 版权说明   : Copyright (c)
 * 其    他     : 
 * 修改日志   : 
***********************************************************************************/


#ifndef __GPRS_SEND_TASK_H__
#define __GPRS_SEND_TASK_H__


#include "main.h"

#define GPRS_SEND_BUF_SIZE	(FRAME_MAX_SIZE)

typedef struct
{
	uint8_t *pdata;
	uint32_t *num;
}GPRS_Send_Frame_t;



void initGPRS_Send_Task(uint8_t **psenddata);
void GPRS_Send_Start(uint8_t *pcount);
void GPRS_Resend_Start(void);
void GPRS_RecvTimeout_Process(void);


#endif

