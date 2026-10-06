/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define DO2_Pin GPIO_PIN_5
#define DO2_GPIO_Port GPIOE
#define DO3_Pin GPIO_PIN_6
#define DO3_GPIO_Port GPIOE
#define DO4_Pin GPIO_PIN_13
#define DO4_GPIO_Port GPIOC
#define DO5_Pin GPIO_PIN_14
#define DO5_GPIO_Port GPIOC
#define DO6_Pin GPIO_PIN_15
#define DO6_GPIO_Port GPIOC
#define DO7_Pin GPIO_PIN_0
#define DO7_GPIO_Port GPIOF
#define DO8_Pin GPIO_PIN_1
#define DO8_GPIO_Port GPIOF
#define DO9_Pin GPIO_PIN_2
#define DO9_GPIO_Port GPIOF
#define DO10_Pin GPIO_PIN_3
#define DO10_GPIO_Port GPIOF
#define DO11_Pin GPIO_PIN_4
#define DO11_GPIO_Port GPIOF
#define DO12_Pin GPIO_PIN_5
#define DO12_GPIO_Port GPIOF
#define DO13_Pin GPIO_PIN_6
#define DO13_GPIO_Port GPIOF
#define DO14_Pin GPIO_PIN_7
#define DO14_GPIO_Port GPIOF
#define DO15_Pin GPIO_PIN_8
#define DO15_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOF
#define LED2_Pin GPIO_PIN_10
#define LED2_GPIO_Port GPIOF
#define TELEPLOT_TX_Pin GPIO_PIN_10
#define TELEPLOT_TX_GPIO_Port GPIOB
#define TELEPLOT_RX_Pin GPIO_PIN_11
#define TELEPLOT_RX_GPIO_Port GPIOB
#define CAN_AUX_RX_Pin GPIO_PIN_12
#define CAN_AUX_RX_GPIO_Port GPIOB
#define CAN_AUX_TX_Pin GPIO_PIN_13
#define CAN_AUX_TX_GPIO_Port GPIOB
#define DI14_Pin GPIO_PIN_2
#define DI14_GPIO_Port GPIOG
#define DI13_Pin GPIO_PIN_3
#define DI13_GPIO_Port GPIOG
#define DI12_Pin GPIO_PIN_4
#define DI12_GPIO_Port GPIOG
#define DI11_Pin GPIO_PIN_5
#define DI11_GPIO_Port GPIOG
#define DI10_Pin GPIO_PIN_6
#define DI10_GPIO_Port GPIOG
#define DI9_Pin GPIO_PIN_7
#define DI9_GPIO_Port GPIOG
#define DI8_Pin GPIO_PIN_8
#define DI8_GPIO_Port GPIOG
#define LIN_TX_Pin GPIO_PIN_6
#define LIN_TX_GPIO_Port GPIOC
#define LIN_RX_Pin GPIO_PIN_7
#define LIN_RX_GPIO_Port GPIOC
#define CAN_MAIN_RX_Pin GPIO_PIN_11
#define CAN_MAIN_RX_GPIO_Port GPIOA
#define CAN_MAIN_TX_Pin GPIO_PIN_12
#define CAN_MAIN_TX_GPIO_Port GPIOA
#define DI_INT_REQUEST_Pin GPIO_PIN_0
#define DI_INT_REQUEST_GPIO_Port GPIOD
#define DI0_Pin GPIO_PIN_1
#define DI0_GPIO_Port GPIOD
#define DI1_Pin GPIO_PIN_3
#define DI1_GPIO_Port GPIOD
#define DI2_Pin GPIO_PIN_4
#define DI2_GPIO_Port GPIOD
#define SERIAL_TX_Pin GPIO_PIN_5
#define SERIAL_TX_GPIO_Port GPIOD
#define SERIAL_RX_Pin GPIO_PIN_6
#define SERIAL_RX_GPIO_Port GPIOD
#define DI3_Pin GPIO_PIN_10
#define DI3_GPIO_Port GPIOG
#define DI4_Pin GPIO_PIN_11
#define DI4_GPIO_Port GPIOG
#define DI5_Pin GPIO_PIN_12
#define DI5_GPIO_Port GPIOG
#define DI6_Pin GPIO_PIN_13
#define DI6_GPIO_Port GPIOG
#define DI7_Pin GPIO_PIN_15
#define DI7_GPIO_Port GPIOG
#define DO0_Pin GPIO_PIN_0
#define DO0_GPIO_Port GPIOE
#define DO1_Pin GPIO_PIN_1
#define DO1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
