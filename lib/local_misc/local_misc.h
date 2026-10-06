/**
 ******************************************************************************
 * @file           : local_misc.h
 * @brief          : Header for local_misc.c file.
 *                   This file contains the misc funcs and utils of the
 *                   application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOCAL_MISC_H
#define LOCAL_MISC_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <circular_buffer.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdbool.h>
#include <main_export.h>
#include "usart.h"
#include "tim.h"
#include "adc.h"

    /* Exported macro ------------------------------------------------------------*/

    /* Exported types ------------------------------------------------------------*/

    /* Exported constants --------------------------------------------------------*/

    /* Exported macro ------------------------------------------------------------*/

    /* Exported functions prototypes ---------------------------------------------*/
    uint32_t median_filter(uint32_t *buffer, uint8_t size);
    bool array_max_safe(const uint32_t *arr, uint32_t size, uint32_t *result);
    bool array_min_safe(const uint32_t *arr, uint32_t size, uint32_t *result);
    // todo: rename void PWM_SetDutyCycle(TIM_HandleTypeDef *htim, uint32_t channel, float duty_percent)
    void set_PWM_duty16(TIM_HandleTypeDef *htim, uint32_t Channel, uint8_t duty_percent); // 16 bit register
    void set_PWM_duty32(TIM_HandleTypeDef *htim, uint32_t Channel, uint8_t duty_percent); // 32 bit register T2/T5

#ifdef __cplusplus
}
#endif

#endif /* LOCAL_MISC_H */
