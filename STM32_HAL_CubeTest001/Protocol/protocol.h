

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "main.h"

#define G2_SERVER_READ_TIMEOUT 1000
#define G2_SERVER_WRITE_TIMEOUT 0x2fff

typedef enum
{
    PROTOCOL_OK = 0,			// 0
    PROTOCOL_ERROR_DEVICE_ID,	// 1
    PROTOCOL_ERROR_CODE,		// 2
    PROTOCOL_ERROR_HEADER,		// 3
    PROTOCOL_ERROR_CMD_ID,		// 4
    PROTOCOL_ERROR_CMD,			// 5
    PROTOCOL_ERROR_CMD_STATUS,	// 6
    PROTOCOL_ERROR_VERSION,		// 7
    PROTOCOL_ERROR_LENGTH,		// 8
    PROTOCOL_ERROR_CRC,			// 9
    PROTOCOL_NOT_SUPPORT = 255,
}PROTOCOL_E;

	
typedef enum
{
	UPDATE_FLOW_START = 0,
	UPDATE_FLOW_DOWNLOAD_INFO,
	UPDATE_FLOW_DOWNLOAD_DATA,
	UPDATE_FLOW_END,
	UPDATE_FLOW_END_1,
}PROTOCOL_FLOW_E;

	
	/* ��� IAP ���Э������ */
#define G2_SERVER_MESSAGE_NEW_FIRMWARE_REQ				0x24
#define G2_SERVER_MESSAGE_UPDATE_FIRMWARE_BEFORE		0x25
#define G2_SERVER_MESSAGE_UPDATE_FIRMWARE				0x26
#define G2_SERVER_MESSAGE_UPDATE_FIRMWARE_AFTER			0x27


#define G2_SERVER_MAX_BUFFER_SIZE 			(USED_DATA_MAX_SIZE)
#define G2_SERVER_MESSAGE_OPERATION_READ 	0x3
#define G2_SERVER_MESSAGE_OPERATION_WRITE 	0x16
#define G2_SERVER_MESSAGE_OPERATION_DELETE 	0x17
#define G2_SERVER_OPERATION_CODE_WRITE 		0x51
#define G2_SERVER_OPERATION_CODE_DELETE 	0x52

#define G2_SERVER_MESSAGE_OPERATION_READ_ACK 	0x83
#define G2_SERVER_MESSAGE_OPERATION_WRITE_ACK 	0x96

#define  G2_SERVER_MESSAGE_PACKET_HEADER	0x40

#define G2_SERVER_PROTOCOL_DEVICE_ID APP_CONFIG_device_id()
#define G2_SERVER_HEADER_SIZE 13
#define G2_SERVER_PROTOCOL_BUFFER_SIZE G2_SERVER_MAX_BUFFER_SIZE + G2_SERVER_HEADER_SIZE + 4

#define crc16(buf, size) get_modbus_crc16(buf, size)
#define crc16_add(buf, size, input)	add_modbus_crc16(buf, size, input)

#define G2_SERVER_CHECK_RC \
if (rc != PROTOCOL_OK)     \
{                          \
    return rc;               \
}

#pragma pack(1)

typedef struct 
{
    uint32_t device_id;  /* �豸ID */ 
    uint8_t code;  /* ������ */ 
    uint8_t header;  /* ֡ͷ */ 
    uint16_t cmd_id;  /* ָ��ID */ 
    uint8_t cmd;  /* ������ */ 
    uint8_t cmd_status;  /* ������״̬ */ 
    uint8_t reversed;  /* Ԥ�� */ 
    uint8_t version;  /* �汾 */ 
    uint8_t length;  /* ���ݳ��� */ 
    uint8_t buffer[G2_SERVER_MAX_BUFFER_SIZE];  /* ���� */ 
    uint16_t crc;  /* CRC */ 
    uint8_t exception;  /* �쳣 */ 
    void *parsed;
}g2_server_packet_t, *g2_server_packet_pt;


typedef struct
{
  uint8_t file_type;  // MCU�������̼���Ӧ��
} g2_server_new_firmware_message_t, *g2_server_new_firmware_message_pt;

typedef struct
{
  uint8_t update_ack;  // MCU�������̼���Ӧ��
} g2_server_new_firmware_ack_message_t, *g2_server_new_firmware_ack_message_pt;
 


#define FILE_NAME_SIZE	(32)
typedef struct
{
	uint8_t file_type;		// �������Ĺ̼����ͣ���Ϊ 0:Bootloader��1:APP1��2:Parament��
	uint8_t file_name[FILE_NAME_SIZE];	// ���������ļ����������ض���������������
	uint32_t file_len;		// ���������ļ�����
	uint16_t file_crc;		// �������Ĺ̼���CRC16
	uint16_t file_frame_no; // �������Ĺ̼�����֡��
	uint8_t file_version_h; // �������Ĺ̼��汾H
	uint8_t file_version_m; // �������Ĺ̼��汾M
	uint8_t file_version_l; // �������Ĺ��ư汾L
	uint8_t is_force_update;// �Ƿ�ǿ�������̼�
} g2_server_firmware_info_message_t, *g2_server_firmware_info_message_pt;


typedef struct
{
  uint8_t update_ack;  // MCU�������̼���Ӧ��
} g2_server_firmware_info_ack_message_t, *g2_server_firmware_info_ack_message_pt;



typedef struct
{
	uint16_t frame_len;			// ��֡��Ч���ݳ���
	uint16_t total_frame;		// ��֡��
	uint16_t frame_no;			// ��ǰ֡���,��0001��ʼ
	uint16_t frame_crc;			// ��Ч���ݵ�CRC16
	uint32_t offset_address;	// ��֡Ҫ��¼��ƫ�Ƶ�ַ
	uint8_t *pData;				// ��Ч��������ָ��
}g2_server_firmware_frame_message_t, *g2_server_firmware_frame_message_pt;


typedef struct
{
	uint16_t frame_no;			// ��ǰ֡���,��0001��ʼ
	uint8_t update_ack; 		// �� UPDATE_ERROR_TYPE_E ��ȡֵ
}g2_server_update_ack_t, *g2_server_update_ack_pt;


typedef struct
{
	uint16_t file_crc;		// �ļ� CRC16 ��ֵ
	uint8_t is_reboot;		// �������Ƿ���������
}g2_server_firmware_finish_message_t, *g2_server_firmware_finish_message_pt;

typedef struct
{
  uint8_t update_ack;  		// MCU�������̼���Ӧ��
} g2_server_firmware_finish_ack_message_t, *g2_server_firmware_finish_ack_message_pt;



#pragma pack()



uint32_t u32LEtoBE(uint32_t datale);
uint16_t u16LEtoBE(uint16_t datale);
uint32_t u32BEtoLE(uint32_t databe);
uint16_t u16BEtoLE(uint16_t databe);

uint8_t protocol_frame_package(g2_server_packet_pt packet, uint8_t cmd, uint8_t code, uint16_t len);
uint8_t protocol_frame_depackage(g2_server_packet_pt packet, uint8_t *databuf);
uint16_t _calculate_crc(g2_server_packet_pt send_packet);


void CRC16_Test(void);


#endif

