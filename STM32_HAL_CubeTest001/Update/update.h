/***********************************************************************************
 * 文 件 名   : update.h
 * 负 责 人   : Z&L
 * 创建日期   : 2021年10月8日
 * 文件描述   : 升级操作相关头文件
 * 版权说明   : Copyright (c)
 * 其    他     : 
 * 修改日志   : 
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
	UPDATE_NO_ERROR = 0,		// 无错误									0
	UPDATE_ERROR_DEVBUSY,		// 设备暂忙，不能升级							1
	UPDATE_ERROR_FILETYPE,		// 文件类型错误(不识别升级的固件类型)					2
	UPDATE_ERROR_FILENAME,		// 文件名错误(命名不符合规范)						3
	UPDATE_ERROR_FILELEN,		// 文件长度错误(超长)							4
	UPDATE_ERROR_FILECRC,		// 文件CRC校验错误							5
	UPDATE_ERROR_STARTADDR,		// 文件的起始烧录地址错误							6
	UPDATE_ERROR_OFFSET,		// 文件烧录的偏移地址错误							7
	UPDATE_ERROR_FRAMENO,		// 帧号错误									8
	UPDATE_ERROR_FRAMELEN,		// 帧长度错误								9
	UPDATE_ERROR_FRAMECRC,		// 帧CRC校验错误								A
	UPDATE_ERROR_FLASHERASE,	// Flash 擦除失败							B
	UPDATE_ERROR_FLASHWRITE,	// Flash 烧录失败							C
	UPDATE_ERROR_FRAMECMD,		// 帧命令错误								D
	UPDATE_ERROR_RETRYFAIL,		// 重发3次仍然失败								E
	UPDATE_ERROR_ERASEPARA,		// 参数擦除 Flash 失败						F
	UPDATE_ERROR_WRITEPARA,		// 参数写入 Flash 失败						0x10
	UPDATE_ERROR_FAKEFRAME,		// 已处理过的 fake frame 数据					0x11
	UPDATE_ERROR_RESENDOK,		// 已处理过的 resend frame 数据				0x12
	UPDATE_ERROR_MALLOC,        // 申请内存失败                               0x13
	UPDATE_ERROR_REBOOTCNT,		// BOOT重启多次失败							0x14
	UPDATE_ERROR_COPYFACTORY,	// copy APP 到 Factory 失败				0x15

	UPDATE_ERROR_RESEND = 0xFE,	// 重发数据
	UPDATE_ERROR_END1 = 0xFF,	// 升级流程结束

	
}UPDATE_ERROR_TYPE_E;





typedef enum
{
	DEV_UPDATE_FIRMWARE_BUSY = 0,
	DEV_UPDATE_FIRMWARE_FREE,
}UPDATE_STATUS_ACK_E;


typedef enum
{
	MCU_BOOT_FIRMWARE = 0,		// BOOT 固件
	MCU_APP_FIRMWARE,			// APP 固件
	MCU_PARAMENT_FIRMWARE,		// PARAMENT 固件
	UNKONW_FIRMWARE = 255,		// 未知固件类型
}FILE_TYPE_E;


typedef enum
{
	UPDATE_FORCE_NOT = 0,		// 不强制升级
	UPDATE_FORCE,				// 强制升级
}UPDATE_FORCE_E;

typedef enum
{
	REBOOT_FORCE_NOT = 0,		// 不强制重启
	REBOOT_FORCE,				// 强制重启
}REBOOT_FORCE_E;
	
typedef enum
{
	REBOOT_LATER = 0,			// 稍后重启
	REBOOT_NOW, 				// 立刻重启
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
	uint32_t start_address;		// 起始地址
	uint32_t current_address;	// 下次烧录起始偏移地址
	uint32_t current_len;		// 当前已烧录长度
	uint32_t total_len;			// 总的烧录长度
	uint16_t current_frame_no;	// 当前烧录的帧号,从0001开始
	uint16_t current_crc;		// 当前累计的CRC16值
	uint16_t total_frame_no;	// 总的帧数
	uint16_t total_crc;			// 总的CRC16值
	uint8_t file_type;			// 升级的文件类型
	uint8_t update_error;		// 升级过程中的错误类型
	g2_server_firmware_finish_message_t command;
}update_firmware_monitor_t, *update_firmware_monitor_pt;

#define FILE_NAME_SIZE	(32)

typedef struct
{
	uint8_t  filename[FILE_NAME_SIZE];		// 读取到的文件的文件名
	uint32_t totallen;			// 读取到的文件的总长度
	uint32_t readoverlen;		// 已经读取到的字节个数
	uint32_t readlen;			// 单次读取文件的字节个数
	uint32_t readoffset;		// 读取文件的偏移位置
	uint32_t retlen;			// 单次读取成功的字节个数
	uint8_t *readdata;			// 读取到的数据内容指针
}FILE_READ_OPT_t;



void resetUpdateFlow(void);



UPDATE_ERROR_TYPE_E update_flow_process_recv(void);
UPDATE_ERROR_TYPE_E update_flow_process_send(void);
UPDATE_ERROR_TYPE_E update_frame_depacket(g2_server_packet_pt packet, uint8_t *recvdata);
UPDATE_ERROR_TYPE_E update_frame_packet(g2_server_packet_pt packet, uint8_t *senddata);


void init_Update_Packet(g2_server_packet_pt *psend, g2_server_packet_pt *precv);


void printfErrorCnt(void);

#endif

