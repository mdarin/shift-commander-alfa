/**
 ******************************************************************************
 * @file           : main_types.h
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
#ifndef MAIN_TYPES_H
#define MAIN_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <circular_buffer.h>
// #include <EMA.h>
// #include <ExampleLib.h>
#include "adc.h"

/* Exported macro ------------------------------------------------------------*/
#define NUM_PI_CHANNELS 4          // Количетсво каналов PI — Pulse Input
#define NUM_AI_CHANNELS 13         // Количество каналов AI — Analog Input
#define SAMPLES_PER_PI_CHANNEL 4   // Количество семплов на импульсный канал
#define PI_CIRCULAR_BUFFER_SIZE 10 // Размер циклического буфера для каналов PI
#define PEEK_ARRAY_SIZE 5

    /* Exported types ------------------------------------------------------------*/

    // Структура для хранения измерений

    // Импульсный вход (PI — Pulse Input)
    typedef struct pulse_measurement_t
    {
        uint32_t period_us;  // Период
        float frequency_hz;  // Частота сигнала
        uint32_t duty_cycle; // For PWM reserved/not used
        uint32_t rise_time;  // Время фронта
        // uint32_t prev_capture;            // not used now
        // uint32_t last_value;              // not used now
        // uint32_t overflow_count;          // not used now
        volatile uint8_t data_ready_;     // private
        cbuf_handle_t raw_period_handle_; // private, not filtered raw period
        lf_cbuf_handle_t period_handle_;  // filtered and lock free available period
        u_int32_t clk_;                   // private
    } pulse_measurement_t;

    // Дискретный вход(DI — Digital Input) или Дискретный канал ввода.
    typedef struct digital_measurement_t
    {
        // TODO:
        volatile uint8_t state;         // состояние входа
        lf_cbuf_handle_t state_handle_; // filtered and lock free available value
        volatile uint8_t data_ready_;
    } digital_measurement_t;

    //  Аналоговый вход (AI — Analog Input)
    typedef struct analog_measurement_t
    {
        volatile uint32_t measured_value; // последнее измеренное значение(мгновенное)
        uint32_t filtered;                // значение после фильрации EMA
        // EMA_Filter_UFixed filter;
        float32_t alpha;                // коэффициент (0 < alpha ≤ 1)
        lf_cbuf_handle_t value_handle_; // filtered and lock free available value
        volatile uint8_t data_ready_;
    } analog_measurement_t;

    /* Exported constants --------------------------------------------------------*/

    /* Exported macro ------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MAIN_TYPES_H */
