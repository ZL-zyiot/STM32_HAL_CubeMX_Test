/***********************************************************************************
 * �� �� ��   : data_Update_Task.c
 * �� �� ��   : Z&L
 * ��������   : 2021��10��8��
 * �ļ�����   : ����������Դ�ļ�
 * ��Ȩ˵��   : Copyright (c)
 * ��    ��     : 
 * �޸���־   : 
***********************************************************************************/



#include "data_Update_Task.h"



#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "update.h"
#include "fatfs.h"

#include "string.h"

extern uint8_t usart1_start_update;
extern osMessageQId recvFrameQHandle;

uint8_t recvFrameBuf[FRAME_BUF_SIZE] = {0};
uint8_t sendFrameBuf[FRAME_BUF_SIZE] = {0};
g2_server_packet_pt psend = NULL;
g2_server_packet_pt precv = NULL;



FILE_READ_OPT_t readOpt = {0};

/*****************************************************************************
 * �� �� ��  : initFile_Data_Task
 * �� �� ��  : Z&L
 * ��������  : 2021��10��8��
 * ��������  : ��ʼ�����������ļ�����ȡ�ļ���Ϣ
 * �������  : void   
 * �������  : ��
 * �� �� ֵ  : static
 * ���ù�ϵ  : 
 * ��    ��    : 

*****************************************************************************/
static void initFile_Data_Task(void)
{
	uint8_t select = 0;
	uint8_t *pname = NULL;
	uint8_t dir[] = "/";
	FRESULT ret;

	readOpt.readoverlen = 0;
	readOpt.readlen = 0;
	readOpt.readoffset = 0;
	readOpt.retlen = 0;
	readOpt.readdata = NULL;
	
	ret = getFileList(dir);
	if(FR_OK == ret)
	{
		pname = selectFile(select);
		if(pname)
		{
			sprintf(readOpt.filename, "%s", pname);
			readOpt.totallen = getFileSize(pname);
		}
	}

	init_Update_Packet(&psend, &precv);
}



osStatus_t file_Data_GetFileData(void)
{
	uint8_t *pname = readOpt.filename;

	// ���ö�ȡ���Ⱥ�ƫ�Ƴ���
	readOpt.readdata = NULL;
	readOpt.retlen = 0;
	readOpt.readlen = 256;
	readOpt.readoffset = 1024;
	
	uint32_t len = readOpt.readlen;
	uint32_t offset = readOpt.readoffset;
	uint8_t *pdata = NULL;

	readOpt.retlen = getFileData(pname, len, offset, &pdata);
	if(readOpt.retlen)
	{
		readOpt.readdata = pdata;
		readOpt.readoverlen += readOpt.retlen;
		readOpt.readoffset += readOpt.retlen;
		return osOK;
	}
	else
	{
		return osError;
	}
}

/*****************************************************************************
 * �� �� ��  : initUpdateTask
 * �� �� ��  : Z&L
 * ��������  : 2021��10��8��
 * ��������  : ��ʼ����������
 * �������  : uint8_t **psenddata  ָ���ͻ�����ָ���ָ��
               uint8_t **precvdata  ָ����ջ�����ָ���ָ��
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��    : 

*****************************************************************************/
void initUpdateTask(uint8_t **psenddata, uint8_t **precvdata)
{
	*precvdata = recvFrameBuf;
	*psenddata = sendFrameBuf;

	initFile_Data_Task();
}


void waitForFileData(void)
{
	osStatus_t status = osOK;
	uint8_t pro;

	status = file_Data_GetFileData();
	if(osOK == status)
	{
		uint32_t count = readOpt.retlen;
		sendFrameBuf[3] = (count) & 0xFF;
		sendFrameBuf[2] = (count >> 8) & 0xFF;
		sendFrameBuf[1] = (count >> 16) & 0xFF;
		sendFrameBuf[0] = (count >> 24) & 0xFF;
		memcpy(&sendFrameBuf[4], readOpt.readdata, readOpt.retlen);
	}
}


UPDATE_ERROR_TYPE_E data_Updata_Process(void)
{
	UPDATE_ERROR_TYPE_E ret = UPDATE_NO_ERROR;

	// ���� Frame ����
	ret = update_frame_depacket(precv, recvFrameBuf);
	ret = update_flow_process_recv();
	if(UPDATE_NO_ERROR == ret)
	{
		ret = update_flow_process_send();
		if(UPDATE_NO_ERROR == ret)
		{
			update_frame_packet(psend, sendFrameBuf);
		}
		else
		{
		}
	}
	else
	{	
		// ���ڵ���ʱ���������������ź�
		if(1 == usart1_start_update)
		{
			static uint32_t test_count = 0;
			initFile_Data_Task();
			resetUpdateFlow();
			usart1_start_update = 0;
			test_count++;
			printf("\r\ntest count = %d \r\n", test_count);
			printfErrorCnt();
		}
		if(UPDATE_ERROR_RESEND != ret)
		{
			ret = update_flow_process_send();
			if(UPDATE_NO_ERROR == ret)
			{
				update_frame_packet(psend, sendFrameBuf);
			}
			else
			{
			}
		}
	}
	
	return ret;
}


