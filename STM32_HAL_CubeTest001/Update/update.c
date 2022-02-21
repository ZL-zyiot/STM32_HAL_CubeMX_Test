
/***********************************************************************************
 * 文 件 名   : update.c
 * 负 责 人   : Z&L
 * 创建日期   : 2021年10月8日
 * 文件描述   : 升级操作相关源文件
 * 版权说明   : Copyright (c)
 * 其    他     : 
 * 修改日志   : 
***********************************************************************************/


#include "update.h"


#include "string.h"


g2_server_packet_t update_send_packet = {0};
g2_server_packet_t update_recv_packet = {0};
FILE_VERSION_T update_file_version = {0};


#ifdef UPDATE_FIRMWARE_DEBUG
uint8_t file_type = MCU_APP_FIRMWARE;
#else
uint8_t file_type = UNKONW_FIRMWARE;
#endif


// 调试时按照实际的步骤进行初值设置
#ifndef UPDATE_FIRMWARE_DEBUG
static PROTOCOL_FLOW_E update_flow_status = UPDATE_FLOW_START; 
#else
static PROTOCOL_FLOW_E update_flow_status = UPDATE_FLOW_START;//UPDATE_FLOW_DOWNLOAD_INFO;
#endif
extern FILE_READ_OPT_t readOpt;

// 用于监控升级流程中的值
#ifndef UPDATE_FIRMWARE_DEBUG
update_firmware_monitor_t update_firmware_monitor = 
{
	.start_address = 0,
	.current_address = 0,
	.current_len = 0,
	.total_len = 0,
	.current_frame_no = 0,
	.current_crc = 0xFFFF,
	.total_crc = 0xFFFF,
	.update_error = UPDATE_NO_ERROR,
	.command =
	{
		.is_reboot = 0,
	}
};
#else
// 调试时设置的值
update_firmware_monitor_t update_firmware_monitor = 
{
	.start_address = 0x8040000,
	.current_address = 0,
	.current_len = 0,
	.total_len = 0x10,
	.current_frame_no = 0,
	.total_frame_no = 0x0,
	.current_crc = 0xFFFF,
	.total_crc = 0xFFFF,
	.file_type = UNKONW_FIRMWARE,
	.update_error = UPDATE_NO_ERROR,
	.command =
	{
		.is_reboot = 0,
	}
};
#endif

void printfResendLog(PROTOCOL_FLOW_E flow, uint8_t *p)
{
	switch(flow)
	{
		case UPDATE_FLOW_START:
			printf("flow_start !!!\r\n");
			break;
		case UPDATE_FLOW_DOWNLOAD_INFO:
			g2_server_firmware_info_ack_message_pt pinfo = (g2_server_firmware_info_ack_message_pt)p;
			printf("flow_download_info retry !!!\r\n");
			printf("ack = 0x%02X\r\n", pinfo->update_ack);
			break;

		case UPDATE_FLOW_DOWNLOAD_DATA:
			g2_server_update_ack_pt pdata = (g2_server_update_ack_pt)p;
			printf("flow_download_data retry !!!\r\n");
			printf("ack = 0x%02X,\t frame_no = 0x%04X\r\n", pdata->update_ack, u16BEtoLE(pdata->frame_no));
			break;

		default :
			break;
	}
}

static int32_t errorCnt = 0;
void printfErrorCnt(void)
{
	printf("error cnt = %d\r\n", errorCnt);
}

void printfUpdateOk(void)
{
	printf("\r\n");
	printf("*************************\r\n");
	printf("Update firmware OK!!!\r\n");
	printf("*************************\r\n");
	printf("\r\n");
}


extern uint16_t set_send_num;
uint16_t read_once_count = 0;
void resetUpdateFlow(void)
{
	update_flow_status = UPDATE_FLOW_START;
	
	if(set_send_num)
	{
		read_once_count = set_send_num;
	}
	else
	{
		read_once_count = READ_ONCE_COUNT;
	}
}


/*****************************************************************************
 * 函 数 名  : init_Update_Fileversion
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 初始化文件版本号信息
 * 输入参数  : FILE_VERSION_T *fileversion  文件版本号结构体指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void init_Update_Fileversion(FILE_VERSION_T *fileversion)
{
	// 调试时设置为下面的值
	fileversion->file_version_h = 1;
	fileversion->file_version_m = 1;
	fileversion->file_version_l = 1;
}


/*****************************************************************************
 * 函 数 名  : init_Update_New_Fireware
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 初始化新固件升级流程的数据包
 * 输入参数  : uint8_t *new_firmware  信息缓冲区指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void init_Update_New_Firmware(uint8_t *new_firmware)
{
	g2_server_new_firmware_message_pt pmsg = (g2_server_new_firmware_message_pt)new_firmware;
	pmsg->file_type = file_type;
}



/*****************************************************************************
 * 函 数 名  : init_Update_Fileinfo
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 初始化待升级的文件信息数据包
 * 输入参数  : uint8_t *infobuf  信息缓冲区指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void init_Update_Fileinfo(uint8_t *infobuf)
{
	uint32_t count = 0;
	g2_server_firmware_info_message_pt pmsg = (g2_server_firmware_info_message_pt)infobuf;
	
	pmsg->file_type = file_type;
	memset(pmsg->file_name, 0, FILE_NAME_SIZE);
	memcpy(pmsg->file_name, readOpt.filename, FILE_NAME_SIZE);
	pmsg->file_len = u32LEtoBE(readOpt.totallen);
	pmsg->file_crc = u16LEtoBE(0xFFFF);		// 初始值设置为 0xFFFF，后续计算出实际值
	if(readOpt.totallen % read_once_count)
	{
		count = 1 + readOpt.totallen/read_once_count;
	}
	else
	{
		count = readOpt.totallen/read_once_count;
	}
	pmsg->file_frame_no = u16LEtoBE(count);
	init_Update_Fileversion(&update_file_version);
	pmsg->file_version_h = update_file_version.file_version_h;
	pmsg->file_version_m = update_file_version.file_version_m;
	pmsg->file_version_l = update_file_version.file_version_l;
	pmsg->is_force_update = UPDATE_FORCE_NOT;

	// 设置 monitor 的初始值
	update_firmware_monitor.start_address = 0;
	update_firmware_monitor.current_address = 0;
	update_firmware_monitor.current_len = 0;
	update_firmware_monitor.total_len = readOpt.totallen;
	update_firmware_monitor.current_frame_no = 1;
	update_firmware_monitor.current_crc = 0xFFFF;
	update_firmware_monitor.total_frame_no = count;
	update_firmware_monitor.total_crc = 0xFFFF;
	update_firmware_monitor.file_type = file_type;
	update_firmware_monitor.update_error = UPDATE_NO_ERROR;
	update_firmware_monitor.command.file_crc = 0xFFFF;
	update_firmware_monitor.command.is_reboot = REBOOT_LATER;
	
#if 0
	switch(file_type)
	{
		case MCU_BOOT_FIRMWARE:
			break;

		case MCU_APP_FIRMWARE:
			break;

		case MCU_PARAMENT_FIRMWARE:
			break;
			
		default :
			break;
	}
#endif
}


/*****************************************************************************
 * 函 数 名  : init_Update_Filedata
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 初始化待升级的文件内容数据包
 * 输入参数  : uint8_t *databuf  数据缓冲区指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint16_t init_Update_Filedata(uint8_t *databuf)
{
	g2_server_firmware_frame_message_pt pmsg = (g2_server_firmware_frame_message_pt)databuf;
	
	// 设置读取长度和偏移长度
	readOpt.readdata = NULL;
	readOpt.retlen = 0;
//	readOpt.readoffset = 0;
	if(readOpt.readoverlen + read_once_count >= readOpt.totallen)
	{
		readOpt.readlen = readOpt.totallen - readOpt.readoverlen;
	}
	else
	{
		readOpt.readlen = read_once_count;		
	}
	
	uint32_t len = readOpt.readlen;
	uint32_t offset = readOpt.readoffset;
	uint8_t *pdata = NULL;
	uint8_t *pname = readOpt.filename;
	
	readOpt.retlen = getFileData(pname, len, offset, &pdata);
	if(readOpt.retlen)
	{
		readOpt.readdata = pdata;		
		readOpt.readoverlen += readOpt.retlen;
		readOpt.readoffset += readOpt.retlen;
	}
	else
	{
	
	}

	
	uint16_t pos = 0;
	uint16_t framelen = 0;
	uint16_t crc = 0xFFFF;
	uint8_t *address = (uint8_t *)(&pmsg->pData);
	framelen = readOpt.retlen;
	pmsg->frame_len = u16LEtoBE(framelen);
	pos += 2;
	pmsg->total_frame = u16LEtoBE(update_firmware_monitor.total_frame_no);
	pos += 2;
	pmsg->frame_no = u16LEtoBE(update_firmware_monitor.current_frame_no);	// 注意此处的赋值！！！
	update_firmware_monitor.current_frame_no++;
	pos += 2;
	pmsg->frame_crc = u16LEtoBE(crc);
	pos += 2;
	pmsg->offset_address = u32LEtoBE(offset);
	pos += 4;
//	pmsg->pData = &databuf[pos];
	memcpy(address, readOpt.readdata, framelen);
	pos += framelen;
	crc = crc16(address, framelen);
	pmsg->frame_crc = u16LEtoBE(crc);

	update_firmware_monitor.total_crc = crc16_add(address, framelen, update_firmware_monitor.total_crc);
	
	return pos;
}

/*****************************************************************************
 * 函 数 名  : init_Update_Filefinish
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月9日
 * 函数功能  : 初始化升级完成内容数据包
 * 输入参数  : uint8_t *databuf  数据缓冲区指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它    : 

*****************************************************************************/
void init_Update_Filefinish(uint8_t *databuf)
{
	g2_server_firmware_finish_message_pt pmsg = (g2_server_firmware_finish_message_pt)databuf;
	pmsg->file_crc = u16LEtoBE(update_firmware_monitor.total_crc);
	pmsg->is_reboot = REBOOT_NOW;
}


/*****************************************************************************
 * 函 数 名  : init_Update_Packet
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 初始化升级流程中的发送和接收缓冲区的操作指针
 * 输入参数  : g2_server_packet_pt *psend  升级流程的发送数据包指针
               g2_server_packet_pt *precv  升级流程的接收数据包指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void init_Update_Packet(g2_server_packet_pt *psend, g2_server_packet_pt *precv)
{
	*psend = &update_send_packet;
	*precv = &update_recv_packet;
}



/*****************************************************************************
 * 函 数 名  : update_frame_depacket
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 将 recvdata 中的数据解包出来放到 packet 中
 * 输入参数  : g2_server_packet_pt packet  数据包指针
               uint8_t *recvdata           接收到数据缓冲区指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
UPDATE_ERROR_TYPE_E update_frame_depacket(g2_server_packet_pt packet, uint8_t *recvdata)
{
	uint8_t ret = 0;
	
	ret = protocol_frame_depackage(packet, &recvdata[4]);
	
	return ret;
}


/*****************************************************************************
 * 函 数 名  : update_frame_packet
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 将 packet 中的数据打包出来放入 senddata 中
 * 输入参数  : g2_server_packet_pt packet  数据包指针
               uint8_t *senddata           发送数据缓冲区指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
UPDATE_ERROR_TYPE_E update_frame_packet(g2_server_packet_pt packet, uint8_t *senddata)
{
	uint32_t count = G2_SERVER_HEADER_SIZE + getPacketLen(packet) + 2;
	
	senddata[3] = (count) & 0xFF;
	senddata[2] = (count >> 8) & 0xFF;
	senddata[1] = (count >> 16) & 0xFF;
	senddata[0] = (count >> 24) & 0xFF;
	G2_SERVER_PACKET_serialize(packet, &senddata[4]);
}


/*****************************************************************************
 * 函 数 名  : update_frame_init
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 准备下一帧的数据
 * 输入参数  : g2_server_packet_pt packet  数据包指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
UPDATE_ERROR_TYPE_E update_frame_init(g2_server_packet_pt packet)
{
	uint16_t len = 0;
	uint8_t cmd = 0;
	uint8_t code = 0;
	UPDATE_ERROR_TYPE_E ret = UPDATE_NO_ERROR;
	
	switch(update_flow_status)
	{
		case UPDATE_FLOW_START:
			cmd = G2_SERVER_MESSAGE_NEW_FIRMWARE_REQ;
			code = G2_SERVER_MESSAGE_OPERATION_READ;
			init_Update_New_Firmware(packet->buffer);
			len = sizeof(g2_server_new_firmware_message_t);
			break;

		case UPDATE_FLOW_DOWNLOAD_INFO:
			cmd = G2_SERVER_MESSAGE_UPDATE_FIRMWARE_BEFORE;
			code = G2_SERVER_MESSAGE_OPERATION_WRITE;
			init_Update_Fileinfo(packet->buffer);
			len = sizeof(g2_server_firmware_info_message_t);
			break;

		case UPDATE_FLOW_DOWNLOAD_DATA:
			cmd = G2_SERVER_MESSAGE_UPDATE_FIRMWARE;
			code = G2_SERVER_MESSAGE_OPERATION_WRITE;
			len = init_Update_Filedata(packet->buffer);
			break;

		case UPDATE_FLOW_END:
			cmd = G2_SERVER_MESSAGE_UPDATE_FIRMWARE_AFTER;
			code = G2_SERVER_MESSAGE_OPERATION_WRITE;
			init_Update_Filefinish(packet->buffer);
			len = sizeof(g2_server_firmware_finish_message_t);
			break;

		default :
			ret = UPDATE_ERROR_END1;
			break;
	}	
	if(UPDATE_NO_ERROR == ret)
	{
		protocol_frame_package(packet, cmd, code, len);
	}
	
	return ret;
}


uint8_t retryCnt = MAX_RETRY_COUNT;

/*****************************************************************************
 * 函 数 名  : update_frame_check
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 检查接收的应答帧具体命令格式是否正确
 * 输入参数  : g2_server_packet_pt recvpacket  接收数据包指针
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
UPDATE_ERROR_TYPE_E update_frame_check(g2_server_packet_pt recvpacket)
{
	UPDATE_ERROR_TYPE_E ret = UPDATE_NO_ERROR;

	switch(update_flow_status)
	{
		case UPDATE_FLOW_START:			
			// 确认是否可以升级
			if(G2_SERVER_MESSAGE_NEW_FIRMWARE_REQ == recvpacket->cmd)
			{
				g2_server_new_firmware_ack_message_pt pmsg = (g2_server_new_firmware_ack_message_pt)(recvpacket->buffer);
				if(UPDATE_ERROR_DEVBUSY == pmsg->update_ack)
				{
					// 取消本次升级
				}
				else if(UPDATE_NO_ERROR == pmsg->update_ack)
				{
				}
				ret = pmsg->update_ack;
			}
			else
			{
				ret = UPDATE_ERROR_FRAMECMD;
			}
			break;

		case UPDATE_FLOW_DOWNLOAD_INFO:
			if(G2_SERVER_MESSAGE_UPDATE_FIRMWARE_BEFORE == recvpacket->cmd)
			{
				g2_server_firmware_info_ack_message_pt pmsg = (g2_server_firmware_info_ack_message_pt)(recvpacket->buffer);
				if((UPDATE_NO_ERROR == pmsg->update_ack) || (UPDATE_ERROR_RESENDOK == pmsg->update_ack))
				{
					// 调试重发操作时设置
				#if 0
					if(retryCnt)
					{
						ret = UPDATE_ERROR_RESEND;
					}
					else
					{
						ret = UPDATE_ERROR_RETRYFAIL;
					}
				#endif
				}
				else
				{
					if(((UPDATE_ERROR_STARTADDR == pmsg->update_ack)
					|| (UPDATE_ERROR_FILELEN == pmsg->update_ack)
					|| (UPDATE_ERROR_FILENAME == pmsg->update_ack)
					|| (UPDATE_ERROR_FILETYPE == pmsg->update_ack))
					&& (retryCnt))
					{
						ret = UPDATE_ERROR_RESEND;
					}
					else
					{
						ret = pmsg->update_ack;
					}
				}
			}
			else
			{
				ret = UPDATE_ERROR_FRAMECMD;
			}
			break;

		case UPDATE_FLOW_DOWNLOAD_DATA:
			if(G2_SERVER_MESSAGE_UPDATE_FIRMWARE == recvpacket->cmd)
			{
				g2_server_update_ack_pt pmsg = (g2_server_update_ack_pt)(recvpacket->buffer);
				if((UPDATE_NO_ERROR == pmsg->update_ack) || (UPDATE_ERROR_RESENDOK == pmsg->update_ack))
				{
					// 调试重发操作时设置
				#if 0
					if(retryCnt)
					{
						ret = UPDATE_ERROR_RESEND;
					}
					else
					{
						ret = UPDATE_ERROR_RETRYFAIL;
					}
				#endif
				}
				else
				{
					if(((UPDATE_ERROR_OFFSET == pmsg->update_ack) 
					|| (UPDATE_ERROR_FRAMENO == pmsg->update_ack) 
					|| (UPDATE_ERROR_FRAMELEN == pmsg->update_ack) 
					|| (UPDATE_ERROR_FRAMECRC == pmsg->update_ack))
					&& (retryCnt))
					{
						ret = UPDATE_ERROR_RESEND;
					}
					else
					{
						ret = pmsg->update_ack;
					}
				}
			}
			else
			{
				ret = UPDATE_ERROR_FRAMECMD;
			}
			break;

		case UPDATE_FLOW_END:
			if(G2_SERVER_MESSAGE_UPDATE_FIRMWARE_AFTER == recvpacket->cmd)
			{
				g2_server_firmware_finish_ack_message_pt pmsg = (g2_server_firmware_finish_ack_message_pt)(recvpacket->buffer);
				if(UPDATE_NO_ERROR == pmsg->update_ack)
				{
				}
				else
				{
				}
				ret = pmsg->update_ack;
			}
			else
			{
				ret = UPDATE_ERROR_FRAMECMD;
			}
			break;
	}
	update_firmware_monitor.update_error = ret;
	
	return ret;
}



/*****************************************************************************
 * 函 数 名  : update_data_check
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 检查文件的数据是否发送完毕
 * 输入参数  : g2_server_packet_pt recvpacket  接收数据包指针
 * 输出参数  : 无
 * 返 回 值  : UPDATE_FLASH_DATA_CONTINUE:未完毕, UPDATE_FLASH_DATA_FINISH:完毕
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
UPDATE_FLASH_DATA_E update_data_check(g2_server_packet_pt recvpacket)
{
	uint16_t frame_no;
	g2_server_update_ack_pt pmsg = (g2_server_update_ack_pt)(recvpacket->buffer);

	frame_no = u16BEtoLE(pmsg->frame_no) ;
	if(frame_no >= update_firmware_monitor.total_frame_no)
	{
		return UPDATE_FLASH_DATA_FINISH;
	}
	else
	{
		return UPDATE_FLASH_DATA_CONTINUE;
	}
}

/*****************************************************************************
 * 函 数 名  : update_flow_process_recv
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 升级流程中对接收的数据的流程处理
 * 输入参数  : void
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
UPDATE_ERROR_TYPE_E update_flow_process_recv(void)
{
	UPDATE_ERROR_TYPE_E ret = UPDATE_NO_ERROR;
	
	ret = update_frame_check(&update_recv_packet);
	if(UPDATE_NO_ERROR != ret)
	{
		printfResendLog(update_flow_status, update_recv_packet.buffer);
	}
	switch(update_flow_status)
	{
		case UPDATE_FLOW_START:			
			if(UPDATE_NO_ERROR == ret)
			{
				update_flow_status = UPDATE_FLOW_DOWNLOAD_INFO;
			}
			else
			{
				update_flow_status = UPDATE_FLOW_END_1;
			}
			break;

		case UPDATE_FLOW_DOWNLOAD_INFO:			
			if(UPDATE_NO_ERROR == ret)
			{
				retryCnt = MAX_RETRY_COUNT;
				update_flow_status = UPDATE_FLOW_DOWNLOAD_DATA;
			}
			else if(UPDATE_ERROR_RESEND == ret)
			{
				retryCnt--;
			}
			else
			{
				update_flow_status = UPDATE_FLOW_END_1;
			}
			break;

		case UPDATE_FLOW_DOWNLOAD_DATA:
			if(UPDATE_NO_ERROR == ret)
			{
				retryCnt = MAX_RETRY_COUNT;
				if(UPDATE_FLASH_DATA_CONTINUE == update_data_check(&update_recv_packet))
				{
					update_flow_status = UPDATE_FLOW_DOWNLOAD_DATA;
				}
				else
				{
					update_flow_status = UPDATE_FLOW_END;
				}
			}
			else if(UPDATE_ERROR_RESEND == ret)
			{
				retryCnt--;
			}
			else
			{
				update_flow_status = UPDATE_FLOW_END_1;
			}
			break;

		case UPDATE_FLOW_END:
			if(UPDATE_NO_ERROR == ret)
			{
				errorCnt--;
				printfUpdateOk();
			}
			update_flow_status = UPDATE_FLOW_END_1;
			break;

		default :
			break;
	}

	return ret;
}


/*****************************************************************************
 * 函 数 名  : update_flow_process_send
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 组织下一包 packet 数据,切换 flow 到下一流程
 * 输入参数  : void   
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
UPDATE_ERROR_TYPE_E update_flow_process_send(void)
{
	UPDATE_ERROR_TYPE_E ret = UPDATE_NO_ERROR;
	
	ret = update_frame_init(&update_send_packet);
	switch (update_flow_status)
	{
		case UPDATE_FLOW_START:		
			errorCnt++;
			break;

		case UPDATE_FLOW_DOWNLOAD_INFO:
			break;

		case UPDATE_FLOW_DOWNLOAD_DATA:			
			break;

		case UPDATE_FLOW_END:
			break;

		case UPDATE_FLOW_END_1:
			update_flow_status = UPDATE_FLOW_START;
			break;

		default :
			break;
	}

	return ret;
}


