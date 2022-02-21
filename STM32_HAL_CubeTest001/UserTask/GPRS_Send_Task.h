/***********************************************************************************
 * �� �� ��   : GPRS_Send_Task.h
 * �� �� ��   : Z&L
 * ��������   : 2021��10��8��
 * �ļ�����   : GPRS��������ͷ�ļ�
 * ��Ȩ˵��   : Copyright (c)
 * ��    ��     : 
 * �޸���־   : 
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

