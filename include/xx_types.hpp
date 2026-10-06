/**
 ******************************************************************************
 * @file           :app_types.h
 * @brief          : Header forAPP.c file.
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
#ifndef APP_TYPES_HPP
#define APP_TYPES_HPP

// #ifdef __cplusplus
// extern "C"
// {
// #endif

/* Includes ------------------------------------------------------------------*/
#include <main_export.h>
#include <stdint.h>
#include <circular_buffer.h>

/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

// TODO: пример использования класса для этого надо сделать хитрость с CubeMX подставлять ему main.c и потом опять менять на main.cpp
namespace app // Контейнер с определениями, переменными и функциями приложения
{
    class Time
    {
    private:
        int hours;
        int minutes;
        int seconds;

    public:
        Time(int h, int m, int s) : hours(h), minutes(m), seconds(s) {} // объявляем конструктор

        // Объявляем три функции для чтения полей:
        int GetHours() const
        {
            return hours;
        };
        int GetMinutes() const
        {
            return minutes;
        };
        int GetSeconds() const
        {
            return seconds;
        };
        void SomeMethod();
    };

    // Абстрактный параметр для показаний датчиков оборотов
    class AbstractParameter
    {
    public:
        // * здесь скорей всего надо определить все возможные свойства параметров
        // * и потом в конкретных классах-репозиториях реализовывать только те,
        // * что нужны именно этому классу-репозиторию для представленяи парамтера

        // Метод для вывода мгновенного значения параметра в Teleplot as (string: message formated "">varName:1234\r\n")

        // Метод получения значения в видео as float(float32_t)

        // Метод получения единиц измерения(units) as (string)

        // Метод перевода в строку кастомизированный String() as string

        // ? +  это общиме методы которые нужно предусмотреть для коллекции параметров
        // CSV() string
        // JSON() string
        // ValueWithUnit() string
        // Marshal string custom marshaler

        // ! это здесь не нужно
        virtual uint32_t RPM() const;
    };

    // Пример формирования репозитория связки промежуточного хранилища метрик и параметра для логики
    class TachoN2 : AbstractParameter
    {
    private:
        // Конфигурация датчика
#define N2_PULSES_PER_REV 60U // количество импульсов на оборот вала
        uint8_t pulses_per_rev;

        // * возможно придётся делать абстракные базовые классы параметров с virtual методами для конкретной реализации

    public:
        TachoN2() : pulses_per_rev(N2_PULSES_PER_REV) {}; // объявляем конструктор

        // получить период в мкс
        uint32_t Period_us() const
        {
            uint32_t period_us = 0;
            lf_circular_buf_get(shafts_meas[0].period_handle_, &period_us);

            return period_us;
        };

        // получить обороты в минуту
        // uint32_t RPM()
        // {
        //     // TODO:
        //     return 0; // getTime() ? (60000000ul / prd) : 0;
        // };

        // получить частоту в герцах
        uint32_t Frequency_Hz() const
        {
            float32_t frequency = (float32_t)shafts_meas[0].clk_ / (float32_t)((this->Period_us() == 0) ? 1 : this->Period_us());

            return (uint32_t)frequency;
        };

        // Расчёт RPM из частоты (Hz)
        // static inline uint32_t HzToRPM(uint32_t frequency_hz)
        // {
        //     return (frequency_hz * 60U) / N2_PULSES_PER_REV;
        // }

        // метод интерфейса здесь конкретная реализация для этого класса
        uint32_t RPM() const override;

        // The pulse - repetition frequency(PRF) is the number of pulses of a repeating signal in a specific time unit.
    };

    /* Exported constants --------------------------------------------------------*/
}

/* Exported macro ------------------------------------------------------------*/

// #ifdef __cplusplus
// }
// #endif

#endif /*APP_TYPES_HPP */
