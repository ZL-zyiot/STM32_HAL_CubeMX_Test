

#include "protocol.h"
#include "crc16.h"

inline uint32_t APP_CONFIG_device_id(void)
{
    return 0x001E8F20;	// 使用默认ID
}

inline uint32_t u32LEtoBE(uint32_t datale)
{
	uint32_t databe = 0;
	
	databe = datale & 0xFF;
	databe <<= 8;
	databe |= (datale >> 8) & 0xFF;
	databe <<= 8;
	databe |= (datale >> 16) & 0xFF;
	databe <<= 8;
	databe |= (datale >> 24) & 0xFF;

	return databe;
}

inline uint16_t u16LEtoBE(uint16_t datale)
{
	uint16_t databe = 0;

	databe = datale & 0xFF;
	databe <<= 8;
	databe |= (datale >> 8) & 0xFF;

	return databe;
}


inline uint32_t u32BEtoLE(uint32_t databe)
{
	uint32_t datale = 0;
	
	datale = databe & 0xFF;
	datale <<= 8;
	datale |= (databe >> 8) & 0xFF;
	datale <<= 8;
	datale |= (databe >> 16) & 0xFF;
	datale <<= 8;
	datale |= (databe >> 24) & 0xFF;

	return datale;
}

inline uint16_t u16BEtoLE(uint16_t databe)
{
	uint16_t datale = 0;

	datale = databe & 0xFF;
	datale <<= 8;
	datale |= (databe >> 8) & 0xFF;

	return datale;
}


uint16_t getPacketLen(g2_server_packet_pt pt)
{
	uint16_t len = 0;

	len = (pt->reversed << 8) | (pt->length);

	return len;
}


void setPacketLen(g2_server_packet_pt pt, uint16_t len)
{
	pt->reversed = len >> 8;
	pt->length = len & 0xFF;
}



uint16_t packet_serialize(g2_server_packet_pt packet, uint8_t bytes[G2_SERVER_PROTOCOL_BUFFER_SIZE])
{
  uint16_t pos = 0;
  uint16_t len = 0;
  
  write_uint32_t_BE(packet->device_id, &bytes[pos]); 
  pos += 4;
  write_uint8_t_BE(packet->code, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->header, &bytes[pos]); 
  pos += 1;
  write_uint16_t_BE(packet->cmd_id, &bytes[pos]); 
  pos += 2;
  write_uint8_t_BE(packet->cmd, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->cmd_status, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->reversed, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->version, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->length, &bytes[pos]); 
  pos += 1;
  len = getPacketLen(packet);
  memcpy(&bytes[pos], packet->buffer, len); 
  pos += len;
  write_uint16_t(packet->crc, &bytes[pos]); 
  pos += 2;
  
  return pos;
}



static uint16_t _header_deserialize(g2_server_packet_pt packet, uint8_t *databuf)
{
    uint16_t pos = 0;
    packet->device_id = read_uint32_t_BE(&databuf[pos]); 
    pos += 4;
    packet->code = read_uint8_t_BE(&databuf[pos]); 
    pos += 1;
    packet->header = read_uint8_t_BE(&databuf[pos]); 
    pos += 1;
    packet->cmd_id = read_uint16_t_BE(&databuf[pos]); 
    pos += 2;
    packet->cmd = read_uint8_t_BE(&databuf[pos]); 
    pos += 1;
    packet->cmd_status = read_uint8_t_BE(&databuf[pos]); 
    pos += 1;
    packet->reversed = read_uint8_t_BE(&databuf[pos]); 
    pos += 1;
    packet->version = read_uint8_t_BE(&databuf[pos]); 
    pos += 1;
    packet->length = read_uint8_t_BE(&databuf[pos]); 
    pos += 1;
    memcpy(packet->buffer, &databuf[pos], packet->length);
    pos += packet->length;
    packet->crc = read_uint16_t(&databuf[pos]);
    pos += 2;

    return pos;
}

static PROTOCOL_E _find_header(g2_server_packet_pt recv_packet)
{
	uint32_t device_id = G2_SERVER_PROTOCOL_DEVICE_ID;
    if((recv_packet->device_id != device_id) 
    && (recv_packet->device_id != 0))
    {
        return PROTOCOL_ERROR_DEVICE_ID;
    }
    if((recv_packet->code != G2_SERVER_MESSAGE_OPERATION_READ) 
		&& (recv_packet->code != G2_SERVER_MESSAGE_OPERATION_READ_ACK)
    	&& (recv_packet->code != G2_SERVER_MESSAGE_OPERATION_WRITE) 
    	&& (recv_packet->code != G2_SERVER_MESSAGE_OPERATION_WRITE_ACK))
    {
        return PROTOCOL_ERROR_CODE;
    }
    if(recv_packet->header != 0x40)
    {
        return PROTOCOL_ERROR_HEADER;
    }
	
	return PROTOCOL_OK;
}


static PROTOCOL_E _parse_extra(g2_server_packet_pt recv_packet)
{
//    if(recv_packet->length < 0)
//    {
//        return PROTOCOL_ERROR_LENGTH;
//    }
	
	return PROTOCOL_OK;
}


static PROTOCOL_E _parse_verify_crc(g2_server_packet_pt recv_packet)
{
    uint16_t crc = 0;
	uint8_t buf[G2_SERVER_PROTOCOL_BUFFER_SIZE] = {0};
  	uint16_t len = packet_serialize(recv_packet, buf);
    crc = crc16(buf , len - 2);
    if(crc != recv_packet->crc)
    {
        return PROTOCOL_ERROR_CRC;
    }

	return PROTOCOL_OK;
}


static uint16_t _calculate_crc(g2_server_packet_pt send_packet)
{
	uint16_t crc = 0;
	uint8_t buf[G2_SERVER_PROTOCOL_BUFFER_SIZE] = {0};
  	uint16_t len = packet_serialize(send_packet, buf);
    crc = crc16(buf , len - 2);
	
    return crc;
}


uint16_t G2_SERVER_PACKET_serialize(g2_server_packet_pt packet, uint8_t *bytes)
{
  uint16_t pos = 0;
  uint16_t len = 0;
  
  write_uint32_t_BE(packet->device_id, &bytes[pos]); 
  pos += 4;
  write_uint8_t_BE(packet->code, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->header, &bytes[pos]); 
  pos += 1;
  write_uint16_t_BE(packet->cmd_id, &bytes[pos]); 
  pos += 2;
  write_uint8_t_BE(packet->cmd, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->cmd_status, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->reversed, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->version, &bytes[pos]); 
  pos += 1;
  write_uint8_t_BE(packet->length, &bytes[pos]); 
  pos += 1;
  len = getPacketLen(packet);
  memcpy(&bytes[pos], packet->buffer, len); 
  pos += len;
  write_uint16_t(packet->crc, &bytes[pos]); 
  pos += 2;
  return pos;
}


/*****************************************************************************
 * 函 数 名  : protocol_frame_package
 * 负 责 人  : Z&L
 * 创建日期  : 2021年10月8日
 * 函数功能  : 填充数据包内容，组成完整的数据包
 * 输入参数  : g2_server_packet_pt packet  数据包指针
               uint8_t cmd                 命令类型
               uint8_t code                操作码
               uint8_t len                 有效数据包长
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint8_t protocol_frame_package(g2_server_packet_pt packet, uint8_t cmd, uint8_t code, uint16_t len)
{
	packet->device_id = (G2_SERVER_PROTOCOL_DEVICE_ID);
	packet->header = G2_SERVER_MESSAGE_PACKET_HEADER;
	packet->cmd_id = (0x0001);
	packet->version = 0x01;
	
	packet->cmd = cmd;
	packet->code = code;
	setPacketLen(packet, len);
	packet->crc = (_calculate_crc(packet));
	
	return 0;
}



uint8_t protocol_frame_depackage(g2_server_packet_pt packet, uint8_t *databuf)
{
    uint8_t rc = 0;
	uint16_t len = 0;
    _header_deserialize(packet, databuf);
    rc = _find_header(packet);
    G2_SERVER_CHECK_RC
    rc = _parse_extra(packet);
    G2_SERVER_CHECK_RC
    rc = _parse_verify_crc(packet);
    G2_SERVER_CHECK_RC
	len = getPacketLen(packet);
    if(packet->code == G2_SERVER_MESSAGE_OPERATION_WRITE 
    && len > 0 
    && packet->buffer[0] == G2_SERVER_OPERATION_CODE_DELETE )
    {
        packet->code=G2_SERVER_MESSAGE_OPERATION_DELETE;
    }
    packet->exception = ((packet->code & 0x80) == 0x80) ? 1 : 0;
	
    return rc;
}



void CRC16_Test(void)
{
	uint8_t input[4] = {0x31, 0x32, 0x33, 0x34};
	uint16_t crcTotal = 0xFFFF, crcHigh = 0xFFFF, crcLow = 0xFFFF;
	uint8_t tempInput[8] = {0};
	uint16_t tempCrc = 0xFFFF;

	crcTotal = crc16(input, 4);
	crcHigh = crc16(input, 2);
	crcLow = crc16(&input[2], 2);

	printf("crcTotal = %04x\t", crcTotal);
	printf("crcHigh = %04x\t", crcHigh);
	printf("crcLow = %04x\t", crcLow);
	printf("\r\n");

	tempInput[0] = (crcHigh>>8) & 0xFF;
	tempInput[1] = crcHigh & 0xFF;
	tempInput[2] = tempInput[1];
	tempInput[3] = tempInput[0];

	tempInput[4] = (crcLow>>8) & 0xFF;
	tempInput[5] = crcLow & 0xFF;
	tempInput[6] = tempInput[5];
	tempInput[7] = tempInput[4];

	tempCrc = crc16_add(tempInput, 2, crcLow);
	printf("tempCrc = %04x\r\n", tempCrc);
	tempCrc = crc16_add(&tempInput[2], 2, crcLow);
	printf("tempCrc = %04x\r\n", tempCrc);
	tempCrc = crc16_add(&tempInput[4], 2, crcHigh);
	printf("tempCrc = %04x\r\n", tempCrc);
	tempCrc = crc16_add(&tempInput[6], 2, crcHigh);
	printf("tempCrc = %04x\r\n", tempCrc);
	printf("---------------------\r\n");

	tempCrc = crc16_add(input, 2, crcLow);
	printf("tempCrc = %04x\r\n", tempCrc);
	tempCrc = crc16_add(&input[2], 2, crcHigh);
	printf("tempCrc = %04x\r\n", tempCrc);
}

