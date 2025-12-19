/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "camera/OV7670_config.h"
#include "camera/OV7670.h"

#include "lcd/ili9341_config.h"
#include "lcd/fonts.h"
#include "lcd/ili9341.h"

#include "sensor/hc_sr04.h"
#include "fpga_communication/fpga_comm.h"
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
#define BTN_TRIGGER_Pin GPIO_PIN_0
#define BTN_TRIGGER_GPIO_Port GPIOA
#define FSMC_BLK_Pin GPIO_PIN_0
#define FSMC_BLK_GPIO_Port GPIOB
#define FSMC_RES_Pin GPIO_PIN_1
#define FSMC_RES_GPIO_Port GPIOB
#define FPGA_SS_Pin GPIO_PIN_12
#define FPGA_SS_GPIO_Port GPIOB
#define CAMERA_RESET_Pin GPIO_PIN_11
#define CAMERA_RESET_GPIO_Port GPIOD
#define HCSR_TRIG_Pin GPIO_PIN_10
#define HCSR_TRIG_GPIO_Port GPIOA
#define HEART_Pin GPIO_PIN_1
#define HEART_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
