/**
  ******************************************************************************
  * �ļ�����: stm_flash.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2017-03-30
  * ��    ��: �ڲ�Falsh��дʵ��
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F4Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm_flash.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/*׼��д��Ĳ�������*/
#define DATA_32                 ((uint32_t)0x87654321)


/* Ҫ�����ڲ�FLASH����ʼ��ַ */
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_4   
/* Ҫ�����ڲ�FLASH�Ľ�����ַ */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_4  +  GetSectorSize(ADDR_FLASH_SECTOR_4) -1 

/* ˽�б��� ------------------------------------------------------------------*/
uint32_t FirstSector = 0, NbOfSectors = 0, Address = 0;
uint32_t SectorError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
static uint32_t GetSector(uint32_t Address);
static uint32_t GetSectorSize(uint32_t Sector);

/* ������ --------------------------------------------------------------------*/

int InternalFlash_Test(void)
{
  /* ����,ɾ����д������Ƚ��� */
  HAL_FLASH_Unlock();
  
  /* ��ȡҪ�������׸����� */
  FirstSector = GetSector(FLASH_USER_START_ADDR);
  
  /* ��ȡҪ������������Ŀ */
  NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1; 
  
  /* ��ʼ�������ṹ�� */
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FirstSector;
  EraseInitStruct.NbSectors = NbOfSectors;
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  { 
    /* �˴�����ʹ��HAL_FLASH_GetError()����ȡ��������������� */
    return 2;
  }
  Address = FLASH_USER_START_ADDR;
  /* ����wordд�����ݵ��ڲ�flash */
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
  /* ����������Ƕ�ȡ�Ļ���������� */
  HAL_FLASH_Lock(); 
  
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = 0;
  /* ��ȡ������֤�Ƿ��д���������ͬ�������ͬ����ôMemoryProgramStatus=0 */
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
  * ��������: ��������ĵ�ַ���������ڵ�sector
  * �������: Address flash��ַ
  * �� �� ֵ: ��
  * ˵    ��: ��
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
  * ��������: ����������Ż�ȡ������С
  * �������: Sector 
  * �� �� ֵ: ��
  * ˵    ��: ��
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
//�������ƣ�STMFLASH_Erase
//�������ܣ���ָ���������׵�ַ��ʼ������ָ����С��FLASH�ռ�
//����������startAddr :��Ҫ������flash�ռ���׵�ַ������Ϊ�������׵�ַ��
            size: ��Ҫ������flash�ռ��С����λ�ֽڣ�
//�������أ�0 �ɹ� -1 �Ƿ���ַ -2 ָ���������򳬳�flash�Ŀռ��С  -3����ʧ��
***************************************/
int32_t stm32_flashErase(uint32_t startAddr,uint32_t size )
{
	uint32_t sectorSize = 0;
	HAL_FLASH_Unlock();
	NbOfSectors = 0;
	sectorSize = 0;
	/* ��ȡҪ�������׸����� */
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
  /* ��ȡҪ������������Ŀ */
  NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1; 
  
  /* ��ʼ�������ṹ�� */
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
//�������ƣ�stm32_flashWrite
//�������ܣ�д���ݵ�flash
//����������writeAddr Ŀ���׵�ַ dataBuf ���ݻ�����ָ�� size ���ݴ�С(��λ ��)
//�������أ�0 д��ɹ� -1 д���쳣 -2 ��������ʧ��
*******************************************/
int16_t stm32_flashWrite(uint32_t writeAddr,uint32_t* dataBuf,uint16_t size)
{
    uint32_t endAddr = 0;
    HAL_FLASH_Unlock();	//���� 
    if (stm32_flashErase(writeAddr,size*4) == 0)
    {
       while(writeAddr<endAddr)//д����
		{	
			
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, writeAddr,*dataBuf)!=HAL_OK)//д������
			{ 
				return -1;//д���쳣
			}
			writeAddr+=4;//ÿ��д�����ֽڣ����Ե�ַ��4
			dataBuf++;
		} 
    }
    else return -2;//��������ʧ��
   HAL_FLASH_Lock();//����
   return 0;//д��ɹ�
}


//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ
//����ֵ:��Ӧ����.
//static uint16_t STMFLASH_ReadHalfWord(u32 faddr)
//{
//    return *(vu16*)faddr;
//}

//��ȡָ����ַ����(32λ����)
//faddr:����ַ
//����ֵ:��Ӧ����.
static uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(__IO uint32_t *)faddr; 
}

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(4λ)��
void stm32_flashRead(uint32_t readAddr,uint32_t* dataBuf,uint16_t size)
{
    uint16_t i = 0;
    for( i=0;i<size;i++)
	{
		dataBuf[i]=STMFLASH_ReadWord(readAddr);//��ȡ4���ֽ�.
		readAddr+=4;//ƫ��4���ֽ�.	
	}
}

static int16_t stm32_flashWriteTest(uint32_t writeAddr,uint32_t data,uint16_t size)
{
	HAL_FLASH_Unlock();	//���� 
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
	HAL_FLASH_Lock();//����
	return 0;//д��ɹ�
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

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
