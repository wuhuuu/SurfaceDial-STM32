/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "main.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_hid.h"
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

/* USER CODE BEGIN PV */
uint16_t enc_last,enc_now;
uint8_t enc_cw = 2;
uint8_t HID_Buffer[3]={0x00};
uint8_t press_flag=0x00;
extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_TIM1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		enc_now= (uint32_t)(__HAL_TIM_GET_COUNTER(&htim1));
		if( enc_last != enc_now )
			enc_cw = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim1);
		else
			enc_cw = 2;
		enc_last = enc_now;
		
		if( enc_cw == 0 && press_flag == 0x00 )
		{
			HID_Buffer[0] = 0x0A;
			HID_Buffer[1] = 0x3C;
			HID_Buffer[2] = 0x00;
			USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)HID_Buffer, 3);
			HAL_GPIO_WritePin(GPIO_M_GPIO_Port, GPIO_M_Pin, GPIO_PIN_SET);
			HAL_Delay(2);
			HAL_GPIO_WritePin(GPIO_M_GPIO_Port, GPIO_M_Pin, GPIO_PIN_RESET);			
		}
		if( enc_cw == 1 && press_flag == 0x00 )
		{
			HID_Buffer[0] = 0x0A;
			HID_Buffer[1] = 0xC4;
			HID_Buffer[2] = 0xFF;
			USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)HID_Buffer, 3);
			HAL_GPIO_WritePin(GPIO_M_GPIO_Port, GPIO_M_Pin, GPIO_PIN_SET);
			HAL_Delay(2);
			HAL_GPIO_WritePin(GPIO_M_GPIO_Port, GPIO_M_Pin, GPIO_PIN_RESET);			
		}
		if( HAL_GPIO_ReadPin(BUT1_GPIO_Port,BUT1_Pin) == 0 && press_flag == 0x00 )
		{
			HID_Buffer[0] = 0x0A;
			HID_Buffer[1] = 0x01;
			HID_Buffer[2] = 0x00;
			press_flag = 0xff;
			USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)HID_Buffer, 3);
		}
		if( HAL_GPIO_ReadPin(BUT1_GPIO_Port,BUT1_Pin) == 1 && press_flag == 0xff )
		{
			HID_Buffer[0] = 0x0A;
			HID_Buffer[1] = 0x00;
			HID_Buffer[2] = 0x00;
			press_flag = 0x00;
			USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)HID_Buffer, 3);
		}
    /* USER CODE END WHILE */
		
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
