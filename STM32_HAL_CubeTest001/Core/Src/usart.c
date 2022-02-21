/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
#include "string.h"

uint8_t uart6_recvdata = 0;
uint8_t uart6_rxbuf[UART6_RX_MAX_SIZE] = {0};
uint32_t uart6_rxcount = 0;

uint8_t uart6_senddata = 0;
uint8_t uart6_txbuf[UART6_TX_MAX_SIZE] = {0};
uint32_t uart6_txcount = 0;


uint8_t uart1_recvdata = 0;
uint8_t uart1_rxbuf[64] = {0};
uint32_t uart1_rxcount = 0;
uint8_t usart1_start_update = 0;


/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	HAL_UART_Receive_IT(&huart1, &uart1_recvdata, 1);
  /* USER CODE END USART1_Init 2 */

}
/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */
	HAL_UART_Receive_IT(&huart6, &uart6_recvdata, 1);
//  	HAL_UART_Transmit_IT(&huart6, &uart6_senddata, 1);
  /* USER CODE END USART6_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* USART6 clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PG9     ------> USART6_RX
    PG14     ------> USART6_TX
    */
    GPIO_InitStruct.Pin = GPRS_RX_Pin|GPRS_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART6_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration
    PG9     ------> USART6_RX
    PG14     ------> USART6_TX
    */
    HAL_GPIO_DeInit(GPIOG, GPRS_RX_Pin|GPRS_TX_Pin);

    /* USART6 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


void UART6_Printf(uint8_t *data)
{
	HAL_UART_Transmit(&huart6, data, strlen(data), HAL_MAX_DELAY);
}

void GPRS_Send_Data(void)
{
	Frame_Timeout_Timer_Stop();

	HAL_UART_Transmit(&huart6, uart6_txbuf, uart6_txcount, HAL_MAX_DELAY);

	Frame_Timeout_Timer_Start();
}


void GPRS_Send_Data_IT(void)
{	
	uart6_senddata = uart6_txbuf[0];
	HAL_UART_Transmit_IT(&huart6, &uart6_senddata, 1);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint32_t count = 0;

	if(USART6 == huart->Instance)
	{
		count++;
		if(count >= uart6_txcount)
		{
			count = 0; 
//			uart6_txcount = 0;
			if(uart6_txcount)
			{
				Frame_Timeout_Timer_Stop();
				Frame_Timeout_Timer_Start();
			}
		}
		else
		{
			uart6_senddata = uart6_txbuf[count];
			HAL_UART_Transmit_IT(&huart6, &uart6_senddata, 1);
		}
	}
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(USART6 == huart->Instance)
	{
		uart6_rxbuf[uart6_rxcount++] = uart6_recvdata;
		Recvdata_Timeout_Timer_Start();
		HAL_UART_Receive_IT(huart, &uart6_recvdata, 1);
	}
	else if(USART1 == huart->Instance)
	{
		uart1_rxbuf[uart1_rxcount++] = uart1_recvdata;
		Recvdata_Timeout_Timer_Start();
		HAL_UART_Receive_IT(huart, &uart1_recvdata, 1);
		usart1_start_update = 1;
	}
}


uint16_t getInteger(uint8_t *datastring)
{
	uint16_t integer = 0;
	uint8_t temp = 0;

	while(*datastring <= '9')
	{
		if(('0' <= *datastring) && ('9' >= *datastring))
		{
			temp = *datastring - '0';
			integer = 10*integer + temp;
		}
		if(('\r' == *(datastring+1)) && ('\n' == *(datastring+2)))
		{
			break;
		}
		datastring++;
	}

	return integer;
}


uint16_t set_send_num = 0;

void copyUart1ToGPRS(void)
{
	if(NULL != strstr(uart1_rxbuf, "start update "))
	{
		set_send_num = getInteger(&uart1_rxbuf[13]);
		if(set_send_num < 0x80)
		{
			set_send_num = 0;
		}
		
		uart6_rxcount = uart1_rxcount;
		memcpy(uart6_rxbuf, uart1_rxbuf, uart1_rxcount);
	}
	else
	{
		usart1_start_update = 0;
	}
	uart1_rxcount = 0;
	memset(uart1_rxbuf, 0, uart1_rxcount);
}


void clearGPRSRecvData(void)
{
	memset(uart6_rxbuf, 0, uart6_rxcount);
	uart6_rxcount = 0;
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
