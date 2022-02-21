/***********************************************************************************
 * 文 件 名   : GPRS_Recv_Task.c
 * 负 责 人   : Z&L
 * 创建日期   : 2021年10月8日
 * 文件描述   : GPRS数据接收任务源文件
 * 版权说明   : Copyright (c)
 * 其    他     : 
 * 修改日志   : 
***********************************************************************************/


#include "GPRS_Recv_Task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "stdlib.h"
#include "string.h"

#include "usart.h"


extern uint8_t uart6_rxbuf[UART6_RX_MAX_SIZE];
extern uint32_t uart6_rxcount;
extern osSemaphoreId recvDataTimeoutSemHandle;
extern osMessageQId recvFrameQHandle;

GPRS_Recv_Frame_t GPRS_Recv_Frame = {0};




void initGPRS_Recv_Task(void)
{
	GPRS_Recv_Frame.pdata = uart6_rxbuf;
	GPRS_Recv_Frame.num = &uart6_rxcount;
}


uint32_t setGPRS_Recv_Count(uint8_t *pcount)
{
	pcount[3] = (*GPRS_Recv_Frame.num) & 0xFF;
	pcount[2] = (*GPRS_Recv_Frame.num >> 8) & 0xFF;
	pcount[1] = (*GPRS_Recv_Frame.num >> 16) & 0xFF;
	pcount[0] = (*GPRS_Recv_Frame.num >> 24) & 0xFF;

	return (*GPRS_Recv_Frame.num);
}


void GPRS_Recv_Process(void)
{
	// 将GPRS接收到的数据放入Queue中，并通知给Update任务
	osStatus_t status;
	uint8_t *pdata = NULL;
	
	pdata = (uint8_t *)malloc(256);
	if(pdata)
	{
		setGPRS_Recv_Count(pdata);
		memcpy(&pdata[4], GPRS_Recv_Frame.pdata, *GPRS_Recv_Frame.num);
		status = osMessageQueuePut(recvFrameQHandle, pdata, 1, 10);
		if(osOK == status)
		{
			clearGPRSRecvData();
		}
		free(pdata);
	}
}



