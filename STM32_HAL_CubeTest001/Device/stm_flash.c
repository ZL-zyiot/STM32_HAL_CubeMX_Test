/**
  ******************************************************************************
  * 文件名程: stm_flash.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2017-03-30
  * 功    能: 内部Falsh读写实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F4Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm_flash.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/*准备写入的测试数据*/
#define DATA_32                 ((uint32_t)0x87654321)


/* 要擦除内部FLASH的起始地址 */
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_4   
/* 要擦除内部FLASH的结束地址 */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_4  +  GetSectorSize(ADDR_FLASH_SECTOR_4) -1 

/* 私有变量 ------------------------------------------------------------------*/
uint32_t FirstSector = 0, NbOfSectors = 0, Address = 0;
uint32_t SectorError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
static uint32_t GetSector(uint32_t Address);
static uint32_t GetSectorSize(uint32_t Sector);

/* 函数体 --------------------------------------------------------------------*/

int InternalFlash_Test(void)
{
  /* 解锁,删除和写入必须先解锁 */
  HAL_FLASH_Unlock();
  
  /* 获取要擦除的首个扇区 */
  FirstSector = GetSector(FLASH_USER_START_ADDR);
  
  /* 获取要擦除的扇区数目 */
  NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1; 
  
  /* 初始化擦除结构体 */
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FirstSector;
  EraseInitStruct.NbSectors = NbOfSectors;
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  { 
    /* 此处可以使用HAL_FLASH_GetError()来获取扇区擦除代码错误 */
    return 2;
  }
  Address = FLASH_USER_START_ADDR;
  /* 按字word写入数据到内部flash */
  while (Address < FLASH_USER_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    { 
      return 3;
    }
  }
  /* 锁定，如果是读取的话，无需解锁 */
  HAL_FLASH_Lock(); 
  
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = 0;
  /* 读取数据验证是否和写入的数据相同，如果相同，那么MemoryProgramStatus=0 */
  while (Address < FLASH_USER_END_ADDR)
  {
    data32 = *(__IO uint32_t*)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;  
    }

    Address = Address + 4;
  }  
  if (MemoryProgramStatus )
  {
    return -1;
  }
  else
  {
   return 0;
  }
  
}

/**
  * 函数功能: 根据输入的地址给出它所在的sector
  * 输入参数: Address flash地址
  * 返 回 值: 无
  * 说    明: 无
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
  {
    sector = FLASH_SECTOR_11;  
  }

  return sector;
}

/**
  * 函数功能: 根据扇区编号获取扇区大小
  * 输入参数: Sector 
  * 返 回 值: 无
  * 说    明: 无
  */
static uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;

  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}


/***************************************
//函数名称：STMFLASH_Erase
//函数功能：从指定的扇区首地址开始，擦除指定大小的FLASH空间
//函数参数：startAddr :需要擦除的flash空间的首地址（必须为扇区的首地址）
            size: 需要擦除的flash空间大小（单位字节）
//函数返回：0 成功 -1 非法地址 -2 指定擦除区域超出flash的空间大小  -3擦除失败
***************************************/
int32_t stm32_flashErase(uint32_t startAddr,uint32_t size )
{
	uint32_t sectorSize = 0;
	HAL_FLASH_Unlock();
	NbOfSectors = 0;
	sectorSize = 0;
	/* 获取要擦除的首个扇区 */
	FirstSector = GetSector(startAddr);
	while(size)
	{
		sectorSize = GetSectorSize(startAddr);
		NbOfSectors += 1;
		if(size < sectorSize)
		{
			break;
		}
		size -= sectorSize;
		startAddr += sectorSize;
	}
  /* 获取要擦除的扇区数目 */
  NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1; 
  
  /* 初始化擦除结构体 */
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FirstSector;
  EraseInitStruct.NbSectors = NbOfSectors;
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  	{
  		return -1;
  	}
  HAL_FLASH_Lock();
  return 0;
}


/*******************************************
//函数名称：stm32_flashWrite
//函数功能：写数据到flash
//函数参数：writeAddr 目标首地址 dataBuf 数据缓冲区指针 size 数据大小(单位 字)
//函数返回：0 写入成功 -1 写入异常 -2 擦除扇区失败
*******************************************/
int16_t stm32_flashWrite(uint32_t writeAddr,uint32_t* dataBuf,uint16_t size)
{
    uint32_t endAddr = 0;
    HAL_FLASH_Unlock();	//解锁 
    if (stm32_flashErase(writeAddr,size*4) == 0)
    {
       while(writeAddr<endAddr)//写数据
		{	
			
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, writeAddr,*dataBuf)!=HAL_OK)//写入数据
			{ 
				return -1;//写入异常
			}
			writeAddr+=4;//每次写入四字节，所以地址加4
			dataBuf++;
		} 
    }
    else return -2;//擦除扇区失败
   HAL_FLASH_Lock();//上锁
   return 0;//写入成功
}


//读取指定地址的半字(16位数据)
//faddr:读地址
//返回值:对应数据.
//static uint16_t STMFLASH_ReadHalfWord(u32 faddr)
//{
//    return *(vu16*)faddr;
//}

//读取指定地址的字(32位数据)
//faddr:读地址
//返回值:对应数据.
static uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(__IO uint32_t *)faddr; 
}

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
void stm32_flashRead(uint32_t readAddr,uint32_t* dataBuf,uint16_t size)
{
    uint16_t i = 0;
    for( i=0;i<size;i++)
	{
		dataBuf[i]=STMFLASH_ReadWord(readAddr);//读取4个字节.
		readAddr+=4;//偏移4个字节.	
	}
}

static int16_t stm32_flashWriteTest(uint32_t writeAddr,uint32_t data,uint16_t size)
{
	HAL_FLASH_Unlock();	//解锁 
	while(size)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, writeAddr, data) != HAL_OK)
		{
			return -1;
		}
		else
		{
			writeAddr += 4;
			data -= 1;
			size -= 4;
		}
	}
	HAL_FLASH_Lock();//上锁
	return 0;//写入成功
}

static int16_t stm32_flashCompare(uint32_t writeAddr,uint32_t data,uint16_t size)
{
	uint32_t readData = 0;
	while(size)
	{
		readData = STMFLASH_ReadWord(writeAddr);
		if(readData != data)
		{
			return -1;
		}
		writeAddr += 4;
		data -= 1;
		size -= 4;
	}
	return 0;
}

int InternalFlash_Test1(void)
{
	uint32_t testWriteData = 0xFFF;
	uint32_t testStartAddr = ADDR_FLASH_SECTOR_5;
	uint32_t testWriteSize = 0x1000;
	
	if(0 == stm32_flashErase(testStartAddr, testWriteSize))
	{
		if(0 != stm32_flashWriteTest(testStartAddr, testWriteData, testWriteSize))
		{
			printf("STM32 Flash Write Fail\r\n");
			return -2;
		}
		else
		{
			if(0 != stm32_flashCompare(testStartAddr, testWriteData, testWriteSize))
			{
				printf("STM32 Flash Compare Fail\r\n");
				return -3;
			}
			else
			{
				printf("STM32 Flash Compare Pass\r\n");
				return 0;
			}
		}
	}
	else
	{
		printf("STM32 Flash Erase Fail\r\n");
		return -1;
	}
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
