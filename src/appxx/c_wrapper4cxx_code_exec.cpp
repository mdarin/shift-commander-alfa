/**
 * Example how to call C++ code from C layer (from FreeRTOS thread here)
 */
#include <c_wrapper4cxx_code_exec.hpp>
#include "usart.h"
#include <xx_types.hpp>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <emb_float.h>
#include <uart_printf.h>
#include <appxx_tcu_params.hpp>
using namespace app;
using namespace tcu;

// Это пространство или слой программы, в котором уже можно и нужно использовать возможности С++.
app::Time t(12, 24, 45);

// Пример определения метода класса в неймспейсе app
void app::Time::SomeMethod()
{
    // TODO: Здесь можно определить большой метод класса
}

// Пример определения абстрактного(виртуального) метода интерфейса по сути

// Для расчета RPM(оборотов в минуту) на основе частоты импульсов :
// Формула расчета
// RPM = (Частота в Гц × 60) / Количество импульсов на оборот
// Подставляем ваши значения
// Частота = 5421 Гц
// Импульсов на оборот = 60
// 60 секунд в минуте
// RPM = (5421 × 60) / 60 = 5421 Итог
// RPM = 5421 об / мин
// Пояснение
// В данном случае получилось совпадение чисел,
// потому что :
// 60 импульсов на оборот
// 60 секунд в минуте
// Эти два множителя сократились,
// поэтому RPM численно равна частоте в Гц.
// Проверка
// Если за 1 секунду получаем 5421 импульс,
// а на один оборот нужно 60 импульсов, то :
// Оборотов в секунду = 5421 / 60 = 90.35 об / с
// Оборотов в минуту = 90.35 × 60 = 5421 об / мин
uint32_t app::TachoN2::RPM() const
{
    // TODO: здесь реализаця для класса TachoN2 и так можно для каждого класса

    // period_us — период между импульсами в микросекундах
    // RPM = (60 * 1_000_000) / (period_us * pulses_per_rev)
    uint32_t rpm = (60000000U) / (this->Period_us() * this->pulses_per_rev);

    return rpm; // oб/мин
}

// Можно вызывать C++ код из C, обернув его в функции в стиле C.
// Для этого нужно использовать extern "C" в C++ коде.
// Вот как это сделать:
// Функции в стиле C с extern "C"
extern "C"
{
    void c_wrapper4cxx_code_exec(void)
    {
        app::TachoN2 local_n2;                 // а так можно объявлять локальную переменную и сней рабоать в функции
        uint32_t local_n2_rpm = tcu::n2.RPM(); // пример использвания глобального параметра TCU

        // локальные переменные параметров лучше не заводить, это ни к чему

        int local_eng = tcu::eng; // пример использвания глобального параметра TCU
        if (local_eng > (int)local_n2_rpm)
        {
        }

        uart_printf(&huart2, "\r\n-Time-: %d H: %d M : %d M \r\n", t.GetHours(), t.GetMinutes(), t.GetSeconds());
        uart_printf(&huart2, "\r\n-PER-:  n2.Period %5lu us\r\n", local_n2.Period_us());
        uart_printf(&huart2, "\r\n-RPM-:  n2.RPM %5lu\r\n", tcu::n2.RPM());
        uart_printf(&huart2, "\r\n-FREQ-:  n2.Frequency %5lu Hz\r\n", tcu::n2.Frequency_Hz());

        // >varName:1234\n
        uart_printf(&huart3, ">n2period_us:%5lu\r\n", tcu::n2.Period_us());
        uart_printf(&huart3, ">n2freq_hz:%5lu\r\n", tcu::n2.Frequency_Hz());
    }
}
// Далее вызываем эту функцию в Си коде.
// При необходимости в неё можно передать аргументы для взаимодействия с С++ кодом.
