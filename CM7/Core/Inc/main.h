/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

#include "stm32h7xx_ll_usart.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_dma.h"

#include "stm32h7xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EXT_SPI_CS1_Pin GPIO_PIN_2
#define EXT_SPI_CS1_GPIO_Port GPIOE
#define EXT_RST_Pin GPIO_PIN_8
#define EXT_RST_GPIO_Port GPIOI
#define PWR_BTN_Pin GPIO_PIN_13
#define PWR_BTN_GPIO_Port GPIOC
#define EXT_SPI_CS0_Pin GPIO_PIN_6
#define EXT_SPI_CS0_GPIO_Port GPIOF
#define EXT_SPI_SCK_Pin GPIO_PIN_7
#define EXT_SPI_SCK_GPIO_Port GPIOF
#define EXT_SPI_CIPO_Pin GPIO_PIN_8
#define EXT_SPI_CIPO_GPIO_Port GPIOF
#define EXT_SPI_COPI_Pin GPIO_PIN_9
#define EXT_SPI_COPI_GPIO_Port GPIOF
#define EXT_INT_Pin GPIO_PIN_3
#define EXT_INT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/*
 * refresh rate = (COUNT + 1) x SDRAM frequency
 *
 * COUNT = (SDRAM refresh period / Number of rows) - 20
 * 
 * SDRAM refresh period for chip used = 64ms
 * Number of rows = 8192
 * SDRAM frequency = 100MHz
 * refresh rate = 64ms / 8192 = 7.81µs
 * 7.81µs * 100MHz = 781
 * Take 20 cycles off for safety so set the register to 761
 */
#define REFRESH_COUNT       ((uint32_t)761)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
