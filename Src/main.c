
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_hal.h"
#include "fatfs.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
 /**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);  ///不需要库原因：从速度考虑直接使用寄存器更好
	return ch;
}


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

FATFS fs;													/* FatFs文件系统对象 */
FIL file;													/* 文件对象 */
FRESULT f_res;                    /* 文件操作结果 */

char SPIFLASHPath[4];             /* 串行Flash逻辑设备路径 */

static void printf_fatfs_error(FRESULT fresult);

void FatfsMount(void);

void FatfsWrite(void);

void FatfsRead();

BYTE ReadBuffer[512]={0};        /* 读缓冲区 */
BYTE WriteBuffer[]= "Helloworld,欢迎使用Fatfs文件系统 \r\n";/* 写缓冲区*/  

uint32_t fnum;            					  /* 文件成功读写数量 */

uint32_t ReadIndex = 0;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	printf("helloworld...\r\n");
  
	FatfsMount(  );
    
  /* 注销一个FatFS设备：串行FLASH */
//  FATFS_UnLinkDriver(SPIFLASHPath);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		
		FatfsWrite(  );		
		HAL_Delay(3000);
		
		FatfsRead(  );	
		HAL_Delay(3000);
		

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */


void FatfsMount(void)
{
		 /* 注册一个FatFS设备：串行FLASH */
  if(FATFS_LinkDriver(&USER_Driver, SPIFLASHPath) == 0)
  {
    //在串行FLASH挂载文件系统，文件系统挂载时会对串行FLASH初始化
    f_res = f_mount(&fs,(TCHAR const*)SPIFLASHPath,1);
    printf_fatfs_error(f_res);
    /*----------------------- 格式化测试 ---------------------------*/  
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if(f_res == FR_NO_FILESYSTEM)
    {
      printf("》串行FLASH还没有文件系统，即将进行格式化...\r\n");
      /* 格式化 */
      f_res=f_mkfs((TCHAR const*)SPIFLASHPath,0,0);							
      
      if(f_res == FR_OK)
      {
        printf("》串行FLASH已成功格式化文件系统。\r\n");
        /* 格式化后，先取消挂载 */
        f_res = f_mount(NULL,(TCHAR const*)SPIFLASHPath,1);			
        /* 重新挂载	*/			
        f_res = f_mount(&fs,(TCHAR const*)SPIFLASHPath,1);
      }
      else
      {
        printf("《《格式化失败。》》\n");
        while(1);
      }
    }
    else if(f_res!=FR_OK)
    {
      printf("！！串行FLASH挂载文件系统失败。(%d)\r\n",f_res);
      printf_fatfs_error(f_res);
      while(1);
    }
    else
    {
      printf("》文件系统挂载成功，可以进行读写测试\r\n");
    }
	}
}

void FatfsWrite(void)
{
 /*----------------------- 文件系统测试：写测试 -----------------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	printf("****** 即将进行文件写入测试... ******\r\n");	
	
	f_res = f_open(&file, "1234.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( f_res == FR_OK )
	{
		for(uint16_t i = 0; i < 200; ++i)
		{
			printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。f_size = %ld\r\n",f_size(&file));
			/* 将指定存储区内容写入到文件内 */
				
			f_lseek(&file, f_size(&file));
			f_res=f_write(&file,WriteBuffer,sizeof(WriteBuffer),&fnum);
			if(f_res==FR_OK)
			{
				printf("》文件写入成功，写入字节数据：%d\r\n",fnum);
				printf("》向文件写入的数据为：\n%s\r\n",WriteBuffer);
			}
			else
			{
				printf("！！文件写入失败：(%d)\r\n",f_res);
			}    
		}
		/* 不再读写，关闭文件 */
		f_close(&file);
	}
	else
	{	
		printf("！！打开/创建文件失败。\r\n");
	}
}

void FatfsRead()
{
	f_res = f_open(&file, "1234.txt", FA_OPEN_EXISTING | FA_READ); 	 
	
	printf("》读到总字节数据：%ld\r\n",f_size(&file));
	if(f_res == FR_OK)
	{
		printf("》打开文件成功。\n");
		
		while(ReadIndex != f_size(&file))
		{
			f_lseek(&file, ReadIndex);
			
			f_res = f_read(&file, ReadBuffer, sizeof(ReadBuffer), &fnum); 
			if(f_res==FR_OK)
			{
				printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
				printf("》读取得的文件数据为：\n%s \r\n", ReadBuffer);
				ReadIndex += 36;
				
				printf("读到字节数据：%d\r\n",ReadIndex);
			}
			else
			{
				printf("！！文件读取失败：(%d)\r\n",f_res);
			}		
		}
	}
	else
	{
		printf("！！打开文件失败。\r\n");
	}
	/* 不再读写，关闭文件 */
	f_close(&file);
	
	/* 不再使用，取消挂载 */
//	f_res = f_mount(NULL,(TCHAR const*)SPIFLASHPath,1);	
}

/**
  * 函数功能: FatFS文件系统操作结果信息处理.
  * 输入参数: FatFS文件系统操作结果：FRESULT
  * 返 回 值: 无
  * 说    明: 无
  */
static void printf_fatfs_error(FRESULT fresult)
{
  switch(fresult)
  {
    case FR_OK:                   //(0)
      printf("》操作成功。\r\n");
    break;
    case FR_DISK_ERR:             //(1)
      printf("！！硬件输入输出驱动出错。\r\n");
    break;
    case FR_INT_ERR:              //(2)
      printf("！！断言错误。\r\n");
    break;
    case FR_NOT_READY:            //(3)
      printf("！！物理设备无法工作。\r\n");
    break;
    case FR_NO_FILE:              //(4)
      printf("！！无法找到文件。\r\n");
    break;
    case FR_NO_PATH:              //(5)
      printf("！！无法找到路径。\r\n");
    break;
    case FR_INVALID_NAME:         //(6)
      printf("！！无效的路径名。\r\n");
    break;
    case FR_DENIED:               //(7)
    case FR_EXIST:                //(8)
      printf("！！拒绝访问。\r\n");
    break;
    case FR_INVALID_OBJECT:       //(9)
      printf("！！无效的文件或路径。\r\n");
    break;
    case FR_WRITE_PROTECTED:      //(10)
      printf("！！逻辑设备写保护。\r\n");
    break;
    case FR_INVALID_DRIVE:        //(11)
      printf("！！无效的逻辑设备。\r\n");
    break;
    case FR_NOT_ENABLED:          //(12)
      printf("！！无效的工作区。\r\n");
    break;
    case FR_NO_FILESYSTEM:        //(13)
      printf("！！无效的文件系统。\r\n");
    break;
    case FR_MKFS_ABORTED:         //(14)
      printf("！！因函数参数问题导致f_mkfs函数操作失败。\r\n");
    break;
    case FR_TIMEOUT:              //(15)
      printf("！！操作超时。\r\n");
    break;
    case FR_LOCKED:               //(16)
      printf("！！文件被保护。\r\n");
    break;
    case FR_NOT_ENOUGH_CORE:      //(17)
      printf("！！长文件名支持获取堆空间失败。\r\n");
    break;
    case FR_TOO_MANY_OPEN_FILES:  //(18)
      printf("！！打开太多文件。\r\n");
    break;
    case FR_INVALID_PARAMETER:    // (19)
      printf("！！参数无效。\r\n");
    break;
  }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
