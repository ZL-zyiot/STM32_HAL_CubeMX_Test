/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "GPRS_Recv_Task.h"
#include "GPRS_Send_Task.h"
#include "data_Update_Task.h"

#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
//uint32_t getFrameSig = 0;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GPRSSendTask */
osThreadId_t GPRSSendTaskHandle;
const osThreadAttr_t GPRSSendTask_attributes = {
  .name = "GPRSSendTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for updateTask */
osThreadId_t updateTaskHandle;
const osThreadAttr_t updateTask_attributes = {
  .name = "updateTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal2,
};
/* Definitions for GPRSRecvTask */
osThreadId_t GPRSRecvTaskHandle;
const osThreadAttr_t GPRSRecvTask_attributes = {
  .name = "GPRSRecvTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal3,
};
/* Definitions for recvFrameQ */
osMessageQueueId_t recvFrameQHandle;
const osMessageQueueAttr_t recvFrameQ_attributes = {
  .name = "recvFrameQ"
};
/* Definitions for sendFrameQ */
osMessageQueueId_t sendFrameQHandle;
const osMessageQueueAttr_t sendFrameQ_attributes = {
  .name = "sendFrameQ"
};
/* Definitions for recvDataTimeoutSem */
osSemaphoreId_t recvDataTimeoutSemHandle;
const osSemaphoreAttr_t recvDataTimeoutSem_attributes = {
  .name = "recvDataTimeoutSem"
};
/* Definitions for recvFrameSem */
osSemaphoreId_t recvFrameSemHandle;
const osSemaphoreAttr_t recvFrameSem_attributes = {
  .name = "recvFrameSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void sendSemToTask(void)
{
	osSemaphoreRelease(recvDataTimeoutSemHandle);
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Start_GPRSSendTask(void *argument);
void StartupdateTask(void *argument);
void Start_GPRSRecvTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
   while(1)
   	{
   		ToggleLED2();
		HAL_Delay(300);
   	}
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of recvDataTimeoutSem */
  recvDataTimeoutSemHandle = osSemaphoreNew(1, 1, &recvDataTimeoutSem_attributes);

  /* creation of recvFrameSem */
  recvFrameSemHandle = osSemaphoreNew(1, 1, &recvFrameSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of recvFrameQ */
  recvFrameQHandle = osMessageQueueNew (1, 1248, &recvFrameQ_attributes);

  /* creation of sendFrameQ */
  sendFrameQHandle = osMessageQueueNew (1, 1248, &sendFrameQ_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of GPRSSendTask */
  GPRSSendTaskHandle = osThreadNew(Start_GPRSSendTask, NULL, &GPRSSendTask_attributes);

  /* creation of updateTask */
  updateTaskHandle = osThreadNew(StartupdateTask, NULL, &updateTask_attributes);

  /* creation of GPRSRecvTask */
  GPRSRecvTaskHandle = osThreadNew(Start_GPRSRecvTask, NULL, &GPRSRecvTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Start_GPRSSendTask */
/**
* @brief Function implementing the GPRSSendTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_GPRSSendTask */
void Start_GPRSSendTask(void *argument)
{
  /* USER CODE BEGIN Start_GPRSSendTask */
	osStatus_t status;
	uint8_t *pSendData;
	uint8_t pro;
		
	initGPRS_Send_Task(&pSendData);
  /* Infinite loop */
	for(;;)
	{
		status = osMessageQueueGet(sendFrameQHandle, pSendData, &pro, osWaitForever);
		if(osOK == status)
		{
			GPRS_Send_Start(pSendData);
		}
		//    osDelay(1);
	}
  /* USER CODE END Start_GPRSSendTask */
}

/* USER CODE BEGIN Header_StartupdateTask */
/**
* @brief Function implementing the updateTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartupdateTask */
void StartupdateTask(void *argument)
{
  /* USER CODE BEGIN StartupdateTask */
  osStatus_t status;
  uint8_t *pSendData, *pRecvData;
  uint8_t pro;
  UPDATE_ERROR_TYPE_E ret = UPDATE_NO_ERROR;

  initFatfs();
//  SD_GetSDCardInfoTest();
  initUpdateTask(&pSendData, &pRecvData);

  /* Infinite loop */
  for(;;)
  {
	status = osMessageQueueGet(recvFrameQHandle, pRecvData, &pro, osWaitForever);
	if(osOK == status)	
	{
		ToggleLED1();
		ret = data_Updata_Process();
		if((UPDATE_NO_ERROR == ret) || (UPDATE_ERROR_RESEND == ret))
		{
			status = osMessageQueuePut(sendFrameQHandle, pSendData, 1, 10);
		}
	}
//    osDelay(1);
  }
  /* USER CODE END StartupdateTask */
}

/* USER CODE BEGIN Header_Start_GPRSRecvTask */
/**
* @brief Function implementing the GPRSRecvTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_GPRSRecvTask */
void Start_GPRSRecvTask(void *argument)
{
  /* USER CODE BEGIN Start_GPRSRecvTask */
	osStatus_t status;
	initGPRS_Recv_Task();
  
	status = osSemaphoreAcquire(recvDataTimeoutSemHandle, osWaitForever);
	if(osOK == status)
	{
		ToggleLED2();
	}

  /* Infinite loop */
  for(;;)
  {
	status = osSemaphoreAcquire(recvDataTimeoutSemHandle, osWaitForever);
	if(osOK == status)
	{
//		ToggleLED2();
		// 数据帧数据接收完成
		GPRS_Recv_Process();
	}
	
//    osDelay(1000);
  }
  /* USER CODE END Start_GPRSRecvTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
