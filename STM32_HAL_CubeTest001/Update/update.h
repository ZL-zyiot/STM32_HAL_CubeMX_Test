/***********************************************************************************
 * �� �� ��   : update.h
 * �� �� ��   : Z&L
 * ��������   : 2021��10��8��
 * �ļ�����   : �����������ͷ�ļ�
 * ��Ȩ˵��   : Copyright (c)
 * ��    ��     : 
 * �޸���־   : 
***********************************************************************************/


#ifndef __UPDATE_H__
#define __UPDATE_H__

#include "main.h"
#include "protocol.h"
#define UPDATE_FIRMWARE_DEBUG

#define MAX_READ_ONCE_COUNT				(USED_DATA_MAX_SIZE)
#define HALF_MAX_READ_ONCE_COUNT		(MAX_READ_ONCE_COUNT >> 1)
#define ONE_FOURTH_MAX_READ_ONCE_COUNT	(MAX_READ_ONCE_COUNT >> 2)
#define READ_ONCE_COUNT					(ONE_FOURTH_MAX_READ_ONCE_COUNT)
#define MAX_RETRY_COUNT					(3)


typedef enum
{
	UPDATE_NO_ERROR = 0,		// �޴���									0
	UPDATE_ERROR_DEVBUSY,		// �豸��æ����������							1
	UPDATE_ERROR_FILETYPE,		// �ļ����ʹ���(��ʶ�������Ĺ̼�����)					2
	UPDATE_ERROR_FILENAME,		// �ļ�������(���������Ϲ淶)						3
	UPDATE_ERROR_FILELEN,		// �ļ����ȴ���(����)							4
	UPDATE_ERROR_FILECRC,		// �ļ�CRCУ�����							5
	UPDATE_ERROR_STARTADDR,		// �ļ�����ʼ��¼��ַ����							6
	UPDATE_ERROR_OFFSET,		// �ļ���¼��ƫ�Ƶ�ַ����							7
	UPDATE_ERROR_FRAMENO,		// ֡�Ŵ���									8
	UPDATE_ERROR_FRAMELEN,		// ֡���ȴ���								9
	UPDATE_ERROR_FRAMECRC,		// ֡CRCУ�����								A
	UPDATE_ERROR_FLASHERASE,	// Flash ����ʧ��							B
	UPDATE_ERROR_FLASHWRITE,	// Flash ��¼ʧ��							C
	UPDATE_ERROR_FRAMECMD,		// ֡�������								D
	UPDATE_ERROR_RETRYFAIL,		// �ط�3����Ȼʧ��								E
	UPDATE_ERROR_ERASEPARA,		// �������� Flash ʧ��						F
	UPDATE_ERROR_WRITEPARA,		// ����д�� Flash ʧ��						0x10
	UPDATE_ERROR_FAKEFRAME,		// �Ѵ������ fake frame ����					0x11
	UPDATE_ERROR_RESENDOK,		// �Ѵ������ resend frame ����				0x12
	UPDATE_ERROR_MALLOC,        // �����ڴ�ʧ��                               0x13
	UPDATE_ERROR_REBOOTCNT,		// BOOT�������ʧ��							0x14
	UPDATE_ERROR_COPYFACTORY,	// copy APP �� Factory ʧ��				0x15

	UPDATE_ERROR_RESEND = 0xFE,	// �ط�����
	UPDATE_ERROR_END1 = 0xFF,	// �������̽���

	
}UPDATE_ERROR_TYPE_E;





typedef enum
{
	DEV_UPDATE_FIRMWARE_BUSY = 0,
	DEV_UPDATE_FIRMWARE_FREE,
}UPDATE_STATUS_ACK_E;


typedef enum
{
	MCU_BOOT_FIRMWARE = 0,		// BOOT �̼�
	MCU_APP_FIRMWARE,			// APP �̼�
	MCU_PARAMENT_FIRMWARE,		// PARAMENT �̼�
	UNKONW_FIRMWARE = 255,		// δ֪�̼�����
}FILE_TYPE_E;


typedef enum
{
	UPDATE_FORCE_NOT = 0,		// ��ǿ������
	UPDATE_FORCE,				// ǿ������
}UPDATE_FORCE_E;

typedef enum
{
	REBOOT_FORCE_NOT = 0,		// ��ǿ������
	REBOOT_FORCE,				// ǿ������
}REBOOT_FORCE_E;
	
typedef enum
{
	REBOOT_LATER = 0,			// �Ժ�����
	REBOOT_NOW, 				// ��������
}REBOOT_TYPE_E;


typedef enum
{
	UPDATE_FLASH_DATA_CONTINUE = 0,
	UPDATE_FLASH_DATA_FINISH,
}UPDATE_FLASH_DATA_E;

typedef struct
{
	uint8_t file_version_h;
	uint8_t file_version_m;
	uint8_t file_version_l;
}FILE_VERSION_T;


typedef struct
{	
	uint32_t start_address;		// ��ʼ��ַ
	uint32_t current_address;	// �´���¼��ʼƫ�Ƶ�ַ
	uint32_t current_len;		// ��ǰ����¼����
	uint32_t total_len;			// �ܵ���¼����
	uint16_t current_frame_no;	// ��ǰ��¼��֡��,��0001��ʼ
	uint16_t current_crc;		// ��ǰ�ۼƵ�CRC16ֵ
	uint16_t total_frame_no;	// �ܵ�֡��
	uint16_t total_crc;			// �ܵ�CRC16ֵ
	uint8_t file_type;			// �������ļ�����
	uint8_t update_error;		// ���������еĴ�������
	g2_server_firmware_finish_message_t command;
}update_firmware_monitor_t, *update_firmware_monitor_pt;

#define FILE_NAME_SIZE	(32)

typedef struct
{
	uint8_t  filename[FILE_NAME_SIZE];		// ��ȡ�����ļ����ļ���
	uint32_t totallen;			// ��ȡ�����ļ����ܳ���
	uint32_t readoverlen;		// �Ѿ���ȡ�����ֽڸ���
	uint32_t readlen;			// ���ζ�ȡ�ļ����ֽڸ���
	uint32_t readoffset;		// ��ȡ�ļ���ƫ��λ��
	uint32_t retlen;			// ���ζ�ȡ�ɹ����ֽڸ���
	uint8_t *readdata;			// ��ȡ������������ָ��
}FILE_READ_OPT_t;



void resetUpdateFlow(void);



UPDATE_ERROR_TYPE_E update_flow_process_recv(void);
UPDATE_ERROR_TYPE_E update_flow_process_send(void);
UPDATE_ERROR_TYPE_E update_frame_depacket(g2_server_packet_pt packet, uint8_t *recvdata);
UPDATE_ERROR_TYPE_E update_frame_packet(g2_server_packet_pt packet, uint8_t *senddata);


void init_Update_Packet(g2_server_packet_pt *psend, g2_server_packet_pt *precv);


void printfErrorCnt(void);

#endif

