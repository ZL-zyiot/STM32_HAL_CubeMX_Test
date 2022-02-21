/***********************************************************************************
 * �� �� ��   : GPRS_Send_Task.c
 * �� �� ��   : Z&L
 * ��������   : 2021��10��8��
 * �ļ�����   : GPRS���ݷ�������Դ�ļ�
 * ��Ȩ˵��   : Copyright (c)
 * ��    ��     : 
 * �޸���־   : 
***********************************************************************************/


#include "GPRS_Send_Task.h"


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "usart.h"

extern uint8_t uart6_txbuf[UART6_TX_MAX_SIZE];
extern uint32_t uart6_txcount;
uint8_t gprsResendCount = 3;


uint8_t GPRS_Send_Buf[GPRS_SEND_BUF_SIZE];


void initGPRS_Send_Task(uint8_t **psenddata)
{
	*psenddata = GPRS_Send_Buf;
}



void GPRS_Send_Start(uint8_t *pcount)
{

	uart6_txcount = pcount[0];
	uart6_txcount <<= 8;
	uart6_txcount |= pcount[1];
	uart6_txcount <<= 8;
	uart6_txcount |= pcount[2];
	uart6_txcount <<= 8;
	uart6_txcount |= pcount[3];
	memcpy(uart6_txbuf, &pcount[4], uart6_txcount);
	GPRS_Send_Data_IT();

}


void GPRS_Resend_Start(void)
{
	GPRS_Send_Data_IT();
}


void GPRS_RecvTimeout_Process(void)
{
	// ִ�д��������ط��ж�
	if(gprsResendCount > 0)
	{
		GPRS_Resend_Start();
//	ToggleLED1();
		gprsResendCount--;
	}
	else
	{
		printf("Recv Frame Timeout !!!\r\n");
	}
}



