/**
 ******************************************************************************
 * @file           : main_export.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_EXPORT_H
#define MAIN_EXPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <main_types.h>

    /* Exported constants --------------------------------------------------------*/

    /* Exported macro ------------------------------------------------------------*/

    /* Exported functions prototypes ---------------------------------------------*/

    /* Exported variables -------------------------------------------------------*/
    extern uint8_t blink_flag;
    extern pulse_measurement_t shafts_meas[NUM_PI_CHANNELS];
    extern analog_measurement_t car_sens_meas[NUM_AI_CHANNELS];
    extern digital_measurement_t pnps_meas;
    extern uint32_t dma_capture_buffer[NUM_PI_CHANNELS][SAMPLES_PER_PI_CHANNEL];
    extern uint32_t dma_adc_buffer[NUM_AI_CHANNELS];

#ifdef __cplusplus
}
#endif

#endif /* MAIN_EXPORT_H */
