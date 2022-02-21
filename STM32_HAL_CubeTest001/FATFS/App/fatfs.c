/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
#include "string.h"
DIR SDFatDir;
uint32_t bytesWrite = 0;
uint32_t bytesRead = 0;
uint8_t writeTxt[10] = {0};
uint8_t readTxt[4096] = {0};

uint8_t binFileList[4][_MAX_LFN];
uint8_t binFilesCount = 0;

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/* USER CODE BEGIN Application */


void initFatfs(void)
{
	
	// mount file system
	retSD = f_mount(&SDFatFS, "", 0);
	if(retSD)
	{
		printf("mount Fail\r\n");
		Error_Handler();
	}
	else
	{
		printf("mount Pass\r\n");
	}
	
	HAL_Delay(1000);
}


FILINFO fileinfo = {0};

FRESULT usrScanBinFiles(uint8_t *dir)
{
	char *fname = NULL;
	char *p = NULL;
	binFilesCount = 0;
	
	retSD = f_opendir(&SDFatDir, dir);
	if(FR_OK == retSD)
	{
		printf("opendir %s OK\r\n", dir);
		for(; ;)
		{
			retSD = f_readdir(&SDFatDir, &fileinfo);
			if((FR_OK != retSD) || (0 == fileinfo.fname[0]))
			{
				retSD = FR_OK;
				break;
			}
			#if _USE_LFN
				fname = (*fileinfo.fname) ? fileinfo.fname:fileinfo.altname;
			#else
				fname = fileinfo.fname;
			#endif

			p = strstr(fname, ".bin");
			if(NULL != p)
			{
				printf("find .bin file : %s\r\n", fname);
				memcpy(binFileList[binFilesCount], fname, strlen(fname));
				binFilesCount++;
			}
		}
	}
	else
	{
		printf("opendir %s ERROR:%d\r\n", dir, retSD);
	}
	
	return retSD;
}

FRESULT getFileList(uint8_t *dir)
{
	retSD = usrScanBinFiles(dir);
	if(FR_OK == retSD)
	{
		printf("scan %s folder .bin list Pass\r\n", dir);
	}
	else
	{
		printf("scan %s folder .bin list Fail\r\n", dir);
	}
	printf("\r\n");

	return retSD;
}


uint8_t* selectFile(uint8_t select)
{
	uint8_t dir[] = "/";
	uint8_t *fileName = NULL;
	fileName = binFileList[select];
	printf("select file is %s\r\n", fileName);

	return fileName;
}


uint32_t getFileSize(uint8_t* pfn)
{
	uint32_t fileSize = 0;
	
	retSD = f_open(&SDFile, pfn, FA_READ);
	if(FR_OK == retSD)
	{
		printf("open file : %s OK\r\n", pfn);
		fileSize = f_size(&SDFile);
		printf("file %s size = %d Bytes\r\n", pfn, fileSize);
		f_close(&SDFile);
	}
	else
	{
		printf("open file : %s ERROR:%d\r\n", pfn, retSD);
	}

	return fileSize;
}


uint32_t getFileData(uint8_t* pfn, uint32_t len, uint32_t offset, uint8_t **pdata)
{
	bytesRead = 0;
	retSD = f_open(&SDFile, pfn, FA_READ);
	if(FR_OK == retSD)
	{
		f_lseek(&SDFile, offset);
		memset(readTxt, 0, sizeof(readTxt));
		retSD = f_read(&SDFile, readTxt, len, &bytesRead);
		if(FR_OK == retSD)
		{
			*pdata = readTxt;
		}
		f_close(&SDFile);
	}
	else
	{
		printf("open file : %s ERROR:%d\r\n", pfn, retSD);
	}

	return bytesRead;
}




#ifdef FATFS_TEST


char testfilename[] = "Test01234.txt";
const uint8_t newline[] = {0xD, 0xA};

void initFatfsTest(void)
{
	// mount file system
	retSD = f_mount(&SDFatFS, "", 0);
	if(retSD)
	{
		printf("mount Fail\r\n");
		Error_Handler();
	}
	else
	{
		printf("mount Pass\r\n");
	}
	HAL_Delay(3000);
	// create and open file
#if 0
	retSD = f_open(&SDFile, testfilename, FA_OPEN_EXISTING);
	if(FR_OK == retSD)
	{
		printf("create file OK\r\n");
		f_close(&SDFile);
	}
	else
	{
		printf("create file ERROR:%d\r\n", retSD);
		HAL_Delay(100);
	}
#endif
}



void usrFatfsWriteTest(void)
{
	const uint8_t writeString[] = "hell fatfs test111";
	
	initFatfsTest();
	HAL_Delay(100);
	
	retSD = f_open(&SDFile, testfilename, FA_WRITE);
	if(FR_OK == retSD)
	{
		f_lseek(&SDFile, f_size(&SDFile));
		retSD = f_write(&SDFile, writeString, sizeof(writeString), &bytesWrite);
		if(FR_OK == retSD)
		{
			printf("write file data OK\r\n");
			printf("write data is :%s", writeString);
		}
		else
		{
			printf("write file data ERROR:%d\r\n", retSD);
		}
		f_write(&SDFile, newline, sizeof(newline), &bytesWrite);
		f_close(&SDFile);
	}
	else
	{
		printf("open file %s ERROR:%d\r\n", testfilename, retSD);
	}
}


void usrFatfsReadTest(void)
{
	uint8_t dir[] = "/";
	uint8_t *testFileName = NULL;
	uint32_t testFileSize = 0;
	uint32_t readOnceSize = 0;
	
	initFatfsTest();
	HAL_Delay(1000);
	retSD = usrScanBinFiles(dir);
	if(FR_OK == retSD)
	{
		printf("scan %s folder .bin list Pass\r\n", dir);
	}
	else
	{
		printf("scan %s folder .bin list Fail\r\n", dir);
		return;
	}
	printf("\r\n");

	testFileName = binFileList[0];
	printf("test file is %s\r\n", testFileName);
	retSD = f_open(&SDFile, testFileName, FA_READ);
	if(FR_OK == retSD)
	{
		printf("open file : %s OK\r\n", testFileName);
		testFileSize = f_size(&SDFile);
		printf("file %s size = %d Bytes\r\n", testFileName, testFileSize);
		if(testFileSize)
		{
			if(testFileSize > 1024)
			{
				readOnceSize = 1024;
			}
			retSD = f_read(&SDFile, readTxt, readOnceSize, &bytesRead);
			if(FR_OK == retSD)
			{
				printf("read bin data is :\r\n");
				uint16_t i, j, k = 0;
				i = (bytesRead + 1)>>4;
				for(i; i>0; i--)
				{
					for(j=0; j<16; j++)
					{
						printf("%02X ", readTxt[k++]);
					}
					printf("\r\n");
				}
			}
			else
			{
				printf("read bin data ERROR:%d\r\n", retSD);
			}
			retSD = f_read(&SDFile, readTxt, readOnceSize, &bytesRead);
			if(FR_OK == retSD)
			{
				printf("read bin data is :\r\n");
				uint16_t i, j, k = 0;
				i = (bytesRead + 1)>>4;
				for(i; i>0; i--)
				{
					for(j=0; j<16; j++)
					{
						printf("%2X ", readTxt[k++]);
					}
					printf("\r\n");
				}
			}
			else
			{
				printf("read bin data ERROR:%d\r\n", retSD);
			}
		}
		f_close(&SDFile);
	}
	else
	{
		printf("open file : %s ERROR:%d\r\n", testFileName, retSD);
	}
}
#endif
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
