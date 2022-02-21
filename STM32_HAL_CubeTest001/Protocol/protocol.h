

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

	
	/* 添加 IAP 相关协议命令 */
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
    uint32_t device_id;  /* 设备ID */ 
    uint8_t code;  /* 功能码 */ 
    uint8_t header;  /* 帧头 */ 
    uint16_t cmd_id;  /* 指令ID */ 
    uint8_t cmd;  /* 命令字 */ 
    uint8_t cmd_status;  /* 命令字状态 */ 
    uint8_t reversed;  /* 预留 */ 
    uint8_t version;  /* 版本 */ 
    uint8_t length;  /* 数据长度 */ 
    uint8_t buffer[G2_SERVER_MAX_BUFFER_SIZE];  /* 数据 */ 
    uint16_t crc;  /* CRC */ 
    uint8_t exception;  /* 异常 */ 
    void *parsed;
}g2_server_packet_t, *g2_server_packet_pt;


typedef struct
{
  uint8_t file_type;  // MCU对升级固件的应答
} g2_server_new_firmware_message_t, *g2_server_new_firmware_message_pt;

typedef struct
{
  uint8_t update_ack;  // MCU对升级固件的应答
} g2_server_new_firmware_ack_message_t, *g2_server_new_firmware_ack_message_pt;
 


#define FILE_NAME_SIZE	(32)
typedef struct
{
	uint8_t file_type;		// 待升级的固件类型，分为 0:Bootloader、1:APP1、2:Parament、
	uint8_t file_name[FILE_NAME_SIZE];	// 待升级的文件名，按照特定的命名规则命名
	uint32_t file_len;		// 待升级的文件长度
	uint16_t file_crc;		// 待升级的固件的CRC16
	uint16_t file_frame_no; // 待升级的固件的总帧数
	uint8_t file_version_h; // 待升级的固件版本H
	uint8_t file_version_m; // 待升级的固件版本M
	uint8_t file_version_l; // 待升级的估计版本L
	uint8_t is_force_update;// 是否强制升级固件
} g2_server_firmware_info_message_t, *g2_server_firmware_info_message_pt;


typedef struct
{
  uint8_t update_ack;  // MCU对升级固件的应答
} g2_server_firmware_info_ack_message_t, *g2_server_firmware_info_ack_message_pt;



typedef struct
{
	uint16_t frame_len;			// 本帧有效数据长度
	uint16_t total_frame;		// 总帧数
	uint16_t frame_no;			// 当前帧序号,从0001开始
	uint16_t frame_crc;			// 有效数据的CRC16
	uint32_t offset_address;	// 本帧要烧录的偏移地址
	uint8_t *pData;				// 有效数据内容指针
}g2_server_firmware_frame_message_t, *g2_server_firmware_frame_message_pt;


typedef struct
{
	uint16_t frame_no;			// 当前帧序号,从0001开始
	uint8_t update_ack; 		// 从 UPDATE_ERROR_TYPE_E 中取值
}g2_server_update_ack_t, *g2_server_update_ack_pt;


typedef struct
{
	uint16_t file_crc;		// 文件 CRC16 的值
	uint8_t is_reboot;		// 升级完是否立即重启
}g2_server_firmware_finish_message_t, *g2_server_firmware_finish_message_pt;

typedef struct
{
  uint8_t update_ack;  		// MCU对升级固件的应答
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

