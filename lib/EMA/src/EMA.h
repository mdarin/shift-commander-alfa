/**
 ******************************************************************************
 * @file           : ema.h
 * @brief          : Header for ema.c file.
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
#ifndef EMA_H
#define EMA_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <arm_math.h>

    /* Exported macro ------------------------------------------------------------*/

    // Q16.16
    // Q12.20 для большей точности дробной части
    // Q24.8 для большего целого диапазона
#define FIXED_SHIFT 16 // * Можно изменить

    /* Exported types ------------------------------------------------------------*/

    // Используем fixed-point арифметику (UQ16.16) - беззнаковый Q16.16
    typedef uint32_t ufixed_t;

    typedef struct EMA_Filter_UFixed
    {
        ufixed_t value; // Текущее значение EMA в unsigned fixed-point
        ufixed_t alpha; // Коэффициент сглаживания (0 < alpha < 1)
        uint8_t initialized;
    } EMA_Filter_UFixed;
    /* Exported constants --------------------------------------------------------*/

    /* Exported macro ------------------------------------------------------------*/

#define FLOAT_TO_UFIXED(x) ((ufixed_t)((x) * (1 << FIXED_SHIFT)))
#define UFIXED_TO_FLOAT(x) ((float32_t)(x) / (1 << FIXED_SHIFT))

    /* Exported functions prototypes ---------------------------------------------*/
    float32_t ema_simple(float32_t measured_value, float32_t prev_ema, float32_t alpha);

    // TODO: это надо пофиксить и проверить с правильным SCALING(смотри файл в doc!)
    void ema_ufixed_init(EMA_Filter_UFixed *filter, float alpha);
    ufixed_t ema_ufixed_update(EMA_Filter_UFixed *filter, ufixed_t measured_value);
    // ufixed_t ema_ufixed_update_cmsis(EMA_Filter_UFixed *filter, ufixed_t measured_value);
    // Оптимизированная версия для Cortex-M4 с использованием DSP инструкций
    __attribute__((always_inline)) static inline ufixed_t ema_ufixed_update_cmsis(EMA_Filter_UFixed *filter, ufixed_t measured_value)
    {
        if (!filter->initialized)
        {
            filter->value = measured_value;
            filter->initialized = 1;
        }
        else
        {

            // EMA формула для unsigned fixed-point
            // term1 = alpha * measured
            // term2 = (1-alpha) * previous_value
            uint32_t term1, term2;

            // Для term2 нужно вычислить (1-alpha):
            ufixed_t one_minus_alpha = (1UL << FIXED_SHIFT) - filter->alpha;

            // Используем инструкции умножения-накопления
            // Для Cortex-M4 с DSP расширениями
            term1 = __SMLAD(filter->alpha, measured_value, 1UL << (FIXED_SHIFT - 1)) >> FIXED_SHIFT;
            term2 = __SMLAD(one_minus_alpha, filter->value, 1UL << (FIXED_SHIFT - 1)) >> FIXED_SHIFT;

            filter->value = term1 + term2;
        }
        return filter->value;
    }

#ifdef __cplusplus
}
#endif

#endif /* EMA_H */
