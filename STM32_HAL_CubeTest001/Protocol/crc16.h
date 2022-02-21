/***********************************************************************************
 * 文 件 名   : crc16.h
 * 负 责 人   : Z&L
 * 创建日期   : 2021年10月8日
 * 文件描述   : CRC16计算头文件
 * 版权说明   : Copyright (c)
 * 其    他     : 
 * 修改日志   : 
***********************************************************************************/


#ifndef __CRC16_H__
#define __CRC16_H__

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define __DECLARE_DATA_TYPE_CONVERTER(type)       \
type read_##type(uint8_t *buffer);              \
type write_##type(type value, uint8_t *buffer); \
type read_##type##_BE(uint8_t *buffer);         \
type write_##type##_BE(type value, uint8_t *buffer);

__DECLARE_DATA_TYPE_CONVERTER(int8_t)
__DECLARE_DATA_TYPE_CONVERTER(int16_t)
__DECLARE_DATA_TYPE_CONVERTER(int32_t)
__DECLARE_DATA_TYPE_CONVERTER(uint8_t)
__DECLARE_DATA_TYPE_CONVERTER(uint16_t)
__DECLARE_DATA_TYPE_CONVERTER(uint32_t)
__DECLARE_DATA_TYPE_CONVERTER(float)
__DECLARE_DATA_TYPE_CONVERTER(double)

#define __DECLARE_1_BITS_CONVERTER(type)       \
uint8_t get_1_bits_count_##type(type value); \
uint8_t get_1_bits_##type(type value, uint8_t *bits);

__DECLARE_1_BITS_CONVERTER(uint8_t)
__DECLARE_1_BITS_CONVERTER(uint16_t)
__DECLARE_1_BITS_CONVERTER(uint32_t)

#define __DECLARE_GET_INDEX(type) \
int get_index_##type(type *p, uint16_t size, type v);

__DECLARE_GET_INDEX(int8_t)
__DECLARE_GET_INDEX(int16_t)
__DECLARE_GET_INDEX(int32_t)
__DECLARE_GET_INDEX(uint8_t)
__DECLARE_GET_INDEX(uint16_t)
__DECLARE_GET_INDEX(uint32_t)

uint16_t get_default_crc16(uint8_t *data_ptr, size_t len);
uint16_t get_reverse_crc16(uint8_t *data_ptr, size_t len);
uint16_t get_modbus_crc16(uint8_t *data_ptr, size_t len);
uint16_t add_modbus_crc16(uint8_t *data_ptr, size_t len, uint16_t crc_input);
uint32_t crc32(uint8_t *buf, size_t size);
uint8_t uint8_t_to_bcd(uint8_t value);
uint8_t uint8_t_from_bcd(uint8_t bcd);

#ifdef __cplusplus
}
#endif

#endif

