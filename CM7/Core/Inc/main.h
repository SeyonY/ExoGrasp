/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32h7xx_nucleo.h"

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
#define THUMB_Pin GPIO_PIN_4
#define THUMB_GPIO_Port GPIOD
#define INDEX_Pin GPIO_PIN_5
#define INDEX_GPIO_Port GPIOD
#define OTHER_SOL_Pin GPIO_PIN_6
#define OTHER_SOL_GPIO_Port GPIOD
#define PUMP_Pin GPIO_PIN_7
#define PUMP_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define NUM_ADC_CHANNELS 1
#define SAMPLES_PER_CHANNEL 200
#define TOTAL_SAMPLES (NUM_ADC_CHANNELS * SAMPLES_PER_CHANNEL)

typedef enum {
	OPEN=0,
	PUMP_OFF_CLOSED,
	CLOSED
} handState_t;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
