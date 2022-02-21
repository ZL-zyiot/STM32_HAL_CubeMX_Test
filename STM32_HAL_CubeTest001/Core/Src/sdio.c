/**
  ******************************************************************************
  * @file    sdio.c
  * @brief   This file provides code for the configuration
  *          of the SDIO instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sdio.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
HAL_StatusTypeDef sdStatus;
HAL_SD_CardStateTypeDef state;

/* USER CODE END 0 */

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

/* SDIO init function */

void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 24;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */

  /* USER CODE END SDIO_MspInit 0 */
    /* SDIO clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDIO DMA Init */
    /* SDIO_RX Init */
    hdma_sdio_rx.Instance = DMA2_Stream3;
    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(sdHandle,hdmarx,hdma_sdio_rx);

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream6;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(sdHandle,hdmatx,hdma_sdio_tx);

    /* SDIO interrupt Init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();

    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(sdHandle->hdmarx);
    HAL_DMA_DeInit(sdHandle->hdmatx);

    /* SDIO interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspDeInit 1 */

  /* USER CODE END SDIO_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void SD_GetSDCardInfoTest(void)
{
	HAL_SD_CardInfoTypeDef sdCardInfo;
	float GB = 0.0;
	uint32_t MB = 0;
	printf("Get SD Card Info Start...\r\n");
//	state = HAL_SD_GetCardState(&hsd);
//	if(0 == state)
//	{
//		printf("SD Card Prepare OK\r\n");
//	}
//	else
//	{
//		printf("SD Card Prepare Error\r\n");
//		HAL_Delay(1000);
//		while(1);
//	}

	sdStatus = HAL_SD_GetCardInfo(&hsd, &sdCardInfo);
	if(HAL_OK == sdStatus)
	{
		printf( "CardType is ：%d\r\n", sdCardInfo.CardType );
		printf( "CardVersion is %d\r\n", sdCardInfo.CardVersion);
	    printf( "CardBlockSize is ：%d\r\n", sdCardInfo.BlockSize );
	    printf( "RelCardAdd is ：%d\r\n", sdCardInfo.RelCardAdd);
	    printf( "CardClass is ：%d \r\n", sdCardInfo.Class);
	    printf( "CardCapacity is ：%d\r\n", sdCardInfo.BlockNbr);
		MB = sdCardInfo.BlockNbr/2048;
		GB = (float)(MB/1024.0);
	    printf( "CardCapacity is ：%d MB\r\n", MB);
	    printf( "CardCapacity is ：%.2f GB\r\n", GB);
		
	}
}


uint8_t eBuffercmp(uint8_t *data0, uint32_t size)
{
	
	while (size--)
	{
		/* SD卡擦除后的可能值为0xff或0 */
		if ((*data0 != 0xFF) && (*data0 != 0))
		{
			return 1;
		}
		data0++;
	}
	return 0;
}

uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
	while (BufferLength--)
	{
		if(BufferLength & 0x3F == 0)
		{
			printf("buf:0x%08X - 0x%08X\n",*pBuffer1,*pBuffer2);
		}
		if (*pBuffer1 != *pBuffer2)
		{
			return 1;
		}
		pBuffer1++;
		pBuffer2++;
	}
	return 0;
}

void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
	uint32_t index = 0;
	/* 填充数据 */
	for (index = 0; index < BufferLength; index++ )
	{
		pBuffer[index] = index + Offset;
	}
}


__weak void SD_EraseTest(void)
{
}


__weak void SD_ReadWriteTest(void)
{
}

__weak void usrSdTest(void)
{
}


#define TEST_WR_START_ADDRESS		(0x000000)
#define TEST_BLOCK_NUM		(8)
#define TEST_BLOCK_SIZE		(512)
#define TEST_WR_END_ADDRESS			(TEST_WR_START_ADDRESS + 4 * TEST_BLOCK_NUM * TEST_BLOCK_SIZE)

#ifdef SDIO_BSP_TEST

uint8_t testStatus = 0;
uint8_t blockRxBuf[4 * TEST_BLOCK_NUM * TEST_BLOCK_SIZE];
uint8_t blockTxBuf[4 * TEST_BLOCK_NUM * TEST_BLOCK_SIZE];

// 【需要注意】部分SD卡可以直接读写，不需要擦除，擦除可能导致无法写入数据
void SD_EraseTest(void)
{
	sdStatus = HAL_SD_Erase(&hsd, TEST_WR_START_ADDRESS, TEST_WR_END_ADDRESS);
	if(HAL_OK == sdStatus)
	{
		sdStatus = HAL_SD_ReadBlocks(&hsd, blockRxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM, HAL_MAX_DELAY);
		testStatus = eBuffercmp(blockRxBuf, 4 * TEST_BLOCK_NUM * TEST_BLOCK_SIZE);
		if(0 == testStatus)
		{
			printf("SD Erase Test Pass\r\n");
		}
		else
		{
			printf("SD Erase Test Fail\r\n");
		}
	}
	else
	{
		printf("SD Erase Fail\r\n");
	}
}


void SD_ReadWriteTest(void)
{
	Fill_Buffer(blockTxBuf,4 * TEST_BLOCK_NUM * TEST_BLOCK_SIZE, 0x32F1);
	sdStatus = HAL_SD_WriteBlocks(&hsd, blockTxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM, HAL_MAX_DELAY);
	printf("write sdStatus:%d\\rn", sdStatus);
	HAL_Delay(2500);

	sdStatus = HAL_SD_ReadBlocks(&hsd, blockRxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM, HAL_MAX_DELAY);
	printf("read sdStatus:%d\r\n",sdStatus);
	HAL_Delay(2500);
	
	testStatus = Buffercmp(blockTxBuf, blockRxBuf, TEST_BLOCK_NUM * TEST_BLOCK_SIZE);	//比较
	if(testStatus == 0)
	{
    	printf("SD ReadWrite Test Pass\r\n");
	}
	else
	{
  		printf("SD ReadWrite Test Fail\r\n");
	}
}



uint8_t sdBlockRxBuf[TEST_BLOCK_NUM * TEST_BLOCK_SIZE];
uint8_t sdBlockTxBuf[TEST_BLOCK_NUM * TEST_BLOCK_SIZE];

void usrSdTest(void)
{
	uint32_t index = 0;
	uint8_t cmpResult = 0;

	sdStatus = HAL_SD_ReadBlocks_DMA(&hsd, sdBlockRxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM);
//	sdStatus = HAL_SD_ReadBlocks(&hsd, sdBlockRxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM, HAL_MAX_DELAY);
	printf("read sdStatus:%d, errorcode = %x\r\n",sdStatus, hsd.ErrorCode);
	HAL_Delay(5000);
	printf("before Write\r\n");
//	for(index = 0; index < TEST_BLOCK_NUM * TEST_BLOCK_SIZE; index++)
//	{
//		printf("%2x ", sdBlockRxBuf[index]);
//	}
	printf("\r\n");

	printf("fill Write data\r\n");
	for(index = 0; index < TEST_BLOCK_NUM * TEST_BLOCK_SIZE; index++)
	{
		sdBlockTxBuf[index] = (index&0xFF);
//		printf("%2x ", sdBlockTxBuf[index]);		
	}
	printf("\r\n");

	printf("start Write\r\n");
	HAL_SD_WriteBlocks_DMA(&hsd, sdBlockTxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM);
//	HAL_SD_WriteBlocks(&hsd, sdBlockTxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM, HAL_MAX_DELAY);

	HAL_Delay(5000);
	
	printf("after Write\r\n");
	printf("read back data\r\n");
	HAL_SD_ReadBlocks_DMA(&hsd, sdBlockRxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM);
//	HAL_SD_ReadBlocks(&hsd, sdBlockRxBuf, TEST_WR_START_ADDRESS, TEST_BLOCK_NUM, HAL_MAX_DELAY);
	HAL_Delay(5000);

	printf("read data is :\r\n");
//	for(index = 0; index < TEST_BLOCK_NUM * TEST_BLOCK_SIZE; index++)
//	{
//		printf("%2x ", sdBlockRxBuf[index]);
//	}
	printf("\r\n");

	cmpResult = Buffercmp(sdBlockTxBuf, sdBlockRxBuf, TEST_BLOCK_NUM * TEST_BLOCK_SIZE);
	printf("\r\n");
	if(0 == cmpResult)
	{
		printf("usrSdTest Pass\r\n");
	}
	else
	{
		printf("usrSdTest Fail\r\n");
	}
}

#endif
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
