/***********************************************************************************
 * �� �� ��   : GPRS_Recv_Task.h
 * �� �� ��   : Z&L
 * ��������   : 2021��10��8��
 * �ļ�����   : GPRS��������ͷ�ļ�
 * ��Ȩ˵��   : Copyright (c)
 * ��    ��     : 
 * �޸���־   : 
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

