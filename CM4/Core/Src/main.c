/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#define HSEM_ID_1 (1U)
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;

LTDC_HandleTypeDef hltdc;

UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_LTDC_Init(void);
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

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();
  /* Activate HSEM notification for Cortex-M4*/
  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
  /*
  Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
  perform system initialization (system clock config, external memory configuration.. )
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
  /* Clear HSEM flag */
  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  MX_LTDC_Init();
  /* USER CODE BEGIN 2 */
  printf("Cortex-M4 starting up\r\n");
  printf("Waiting for memory access\r\n");

  while (HAL_HSEM_FastTake(HSEM_ID_1) != HAL_OK) {;}

  printf("Taken control of SDRAM2\r\n");

  HAL_GPIO_WritePin(DISPLAY_I2C_RST_GPIO_Port, DISPLAY_I2C_RST_Pin, GPIO_PIN_SET);

  uint8_t rxbuf[2];
  if (HAL_I2C_Mem_Read(&hi2c1, 0x70, 0, I2C_MEMADD_SIZE_8BIT, rxbuf, 2, 0xffff) == HAL_OK)
  {
    printf("Read HDMI tx vendor id as 0x%04x\r\n", (unsigned int)*(uint16_t *)rxbuf);
  }
  else
  {
    printf("I2C error\r\n");
  }

  if(HAL_I2C_Mem_Read(&hi2c1, 0x70, 8, I2C_MEMADD_SIZE_8BIT, rxbuf, 1, 0xffff) == HAL_OK)
  {
    printf("Read HDML CTL as 0x%02x\r\n", (unsigned int)rxbuf[0]);
  }
  else
  {
    printf("I2C error reading initial ctl reg\r\n");
  }

  rxbuf[0] = 0x3f;
  if (HAL_I2C_Mem_Write(&hi2c1, 0x70, 8, I2C_MEMADD_SIZE_8BIT, rxbuf, 1, 0xffff) == HAL_OK)
  {
    printf("HDMI Tx enabled\r\n");
  }
  else
  {
    printf("I2C error\r\n");
  }

  if (HAL_I2C_Mem_Read(&hi2c1, 0x70, 8, I2C_MEMADD_SIZE_8BIT, rxbuf, 1, 0xffff) == HAL_OK)
  {
    printf("Read HDMI CTL as 0x%02x\r\n", (unsigned int)rxbuf[0]);
  }
  else
  {
    printf("I2C error\r\n");
  }

  if (HAL_I2C_Mem_Read(&hi2c1, 0x70, 0x3A, I2C_MEMADD_SIZE_8BIT, rxbuf, 2, 0xffff) == HAL_OK)
  {
    printf("Read H_RES as %d\r\n", (int)*(uint16_t *)rxbuf);
  }
  else
  {
    printf("I2C error reading H_RES\r\n");
  }

  if (HAL_I2C_Mem_Read(&hi2c1, 0x70, 0x3C, I2C_MEMADD_SIZE_8BIT, rxbuf, 2, 0xffff) == HAL_OK)
  {
    printf("Read V_RES as %d\r\n", (int)*(uint16_t *)rxbuf);
  }
  else
  {
    printf("I2C error reading V_RES\r\n");
  }

#define ROW_STRIDE (490 * 4)

  // now write something to the layer buffers
  uint8_t *fb1 = (uint8_t *)0xD0000000;

  for (int i=0;i<145;i++)
  {
    for (int j=0;j<245;j++)
    {
      fb1[i*ROW_STRIDE+j*4] = 0xff;
      fb1[i*ROW_STRIDE+j*4+1] = 0xff;
      fb1[i*ROW_STRIDE+j*4+2] = 0xff;
      fb1[i*ROW_STRIDE+j*4+3] = 0xff;
    }
  }
  for (int i=0;i<145;i++)
  {
    for (int j=245;j<490;j++)
    {
      fb1[i*ROW_STRIDE+j*4] = 0x00;
      fb1[i*ROW_STRIDE+j*4+1] = 0x00;
      fb1[i*ROW_STRIDE+j*4+2] = 0x00;
      fb1[i*ROW_STRIDE+j*4+3] = 0xff;
    }
  }
  for (int i=145;i<290;i++)
  {
    for (int j=0;j<245;j++)
    {
      fb1[i*ROW_STRIDE+j*4] = 0xff;
      fb1[i*ROW_STRIDE+j*4+1] = 0x00;
      fb1[i*ROW_STRIDE+j*4+2] = 0x00;
      fb1[i*ROW_STRIDE+j*4+3] = 0xff;
    }
  }
  for (int i=145;i<290;i++)
  {
    for (int j=245;j<490;j++)
    {
      fb1[i*ROW_STRIDE+j*4] = 0x00;
      fb1[i*ROW_STRIDE+j*4+1] = 0x00;
      fb1[i*ROW_STRIDE+j*4+2] = 0xff;
      fb1[i*ROW_STRIDE+j*4+3] = 0xff;
    }
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x307075B1;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 135;
  hltdc.Init.VerticalSync = 5;
  hltdc.Init.AccumulatedHBP = 295;
  hltdc.Init.AccumulatedVBP = 34;
  hltdc.Init.AccumulatedActiveW = 1319;
  hltdc.Init.AccumulatedActiveH = 802;
  hltdc.Init.TotalWidth = 1343;
  hltdc.Init.TotalHeigh = 805;
  hltdc.Init.Backcolor.Blue = 165;
  hltdc.Init.Backcolor.Green = 110;
  hltdc.Init.Backcolor.Red = 58;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 10;
  pLayerCfg.WindowX1 = 500;
  pLayerCfg.WindowY0 = 10;
  pLayerCfg.WindowY1 = 300;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = 0xd0000000;
  pLayerCfg.ImageWidth = 490;
  pLayerCfg.ImageHeight = 290;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
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
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart6, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart6, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DISPLAY_I2C_RST_GPIO_Port, DISPLAY_I2C_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : DISPLAY_I2C_RST_Pin */
  GPIO_InitStruct.Pin = DISPLAY_I2C_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DISPLAY_I2C_RST_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
