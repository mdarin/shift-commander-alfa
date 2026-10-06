/* Includes ------------------------------------------------------------------*/
#include <xx_types.hpp>
#include "usart.h"
#include <main_export.h>
#include <local_misc.h>
#include <emb_float.h>
#include <uart_printf.h>

// Это пространство или слой программы, в котором уже можно и нужно использовать возможности С++
// Здесь будут только вставки на С

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void print_pi_measurements(void);
void display_pi_measurements(void);
void print_ai_measurements();
void display_ai_measurements(void);

// Здесь код на С не стоит уже активно использовать, следут писать на С++

extern "C"
{
    /**
     * @brief
     * @retval None
     */
    // Точка входа, в этой функции исполняется код зазачи в потоке операционной системы
    void appxx_blinker(uint32_t counter) // (можно передать аргументы, это данные которые можно получать от других потоков ОС)
    {
        app::TachoN2 n2; // todo (meas, channel) для инициализации корректной, а может это лучше в конструкторе предусмотреть?

        if (n2.RPM() < 1000)
        {
            // TODO:
        }

        // teleplot_buff.put(&n2);
        // TODO: здесь код на С++ который реализует логику данной задачи выполняемой в потоке исполнеия RTOS

        // * Out to console

        // Вывод результатов измеренеий
        {

            print_pi_measurements();
            print_ai_measurements();
        }

        // * Out to Teleplot dashboard

        // Every serial message formated >varName:1234\n will be ploted in teleplot.Other messages will be printed in the teleplot console.
        {
            // Пример вывода параметров
            // первый параметрт
            float32_t sin_value = arm_sin_f32(counter);
            uart_printf(&huart3, ">sin:" _F5 "\r\n", FLOAT5(sin_value));
            // второй параметр
            float32_t cos_value = arm_cos_f32(counter);
            uart_printf(&huart3, ">cos:" _F5 "\r\n", FLOAT5(cos_value));
        }
    }
} // end extern "C"

/* Task code -----------------------------------------------------------------*/

// TODO:

/* Private function  ---------------------------------------------------------*/
/**
 * @brief
 * @retval None
 */
// Вывод измерений PI
void print_pi_measurements(void)
{
    static uint32_t print_counter = 0;

    // Выводим не каждое измерение, а например, каждое 10-е
    if (++print_counter >= 50)
    {
        print_counter = 0;
        display_pi_measurements();
    }
}

/**
 * @brief
 * @retval None
 */
// Отображение измерений
void display_pi_measurements(void)
{
    char msg[128] = {0};

    sprintf(msg, "\r\n=== PI Measurements ===\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

    for (int i = 0; i < NUM_PI_CHANNELS; i++)
    {
        sprintf(msg, "CH%d: ", i + 1);
        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

        if (shafts_meas[i].frequency_hz > 0)
        {
            sprintf(msg, "Freq: %5lu Hz, Period: %5lu us", (uint32_t)shafts_meas[i].frequency_hz, shafts_meas[i].period_us);
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

            // // Для каналов 1 и 3 показываем скважность
            // if (i == 0 || i == 2)
            // {
            //   printf(", Duty: %2lu%%", shafts_measduty_cycle[i / 2]);
            // }
        }
        else
        {
            sprintf(msg, "No signal");
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }

        sprintf(msg, "\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }

    sprintf(msg, "=====================\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

    // Пказать состояние измерительнгого буфера для каждого канала
    msg[0] = '\0'; // Просто устанавливаем первый символ как нуль-терминатор

    for (int i = 0; i < NUM_PI_CHANNELS; i++)
    {
        // как только буфер наполнится, отобразим 5 элементов
        if (circular_buf_full(shafts_meas[i].raw_period_handle_))
        {
            int offset = snprintf(msg, sizeof(msg), "CH %i buffer: ", i);
            circular_buf_data_t peek_data[PEEK_ARRAY_SIZE];
            circular_buf_peek(shafts_meas[i].raw_period_handle_, peek_data, PEEK_ARRAY_SIZE);
            for (int j = 0; j < PEEK_ARRAY_SIZE; j++)
            {
                offset += snprintf(msg + offset, sizeof(msg) - offset,
                                   (j == 0) ? " %lu" : ", %lu",
                                   (uint32_t)peek_data[j]);
            }

            // ! DEBUG
            // Возвращаем медиану
            uint32_t median = median_filter((uint32_t *)peek_data, PEEK_ARRAY_SIZE);

            // отступаем от данных измерений
            offset = strlen(msg);
            // формируем итоговую строку
            snprintf(msg + offset, sizeof(msg) - offset, "  median: %lu\n", median);
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }
    }
}

void print_ai_measurements()
{
    static uint32_t print_counter = 0;

    // Выводим не каждое измерение, а например, каждое 10-е
    if (++print_counter >= 50)
    {
        print_counter = 0;
        display_ai_measurements();
    }
}

/**
 * @brief
 * @retval None
 */
// Отображение измерений
void display_ai_measurements(void)
{
    char msg[128] = {0};

    // Out to console
    sprintf(msg, "\r\n=== ADC Measurements ===\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

    // Обработка данных для каждого канала
    for (int channel = 0; channel < NUM_AI_CHANNELS; channel++)
    {
        // Out to console
        char msg[128] = {0};
        sprintf(msg, "CH%02u: ", channel);
        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

        if (car_sens_meas[channel].filtered > 0)
        {
            // * Перевод сырого значения АЦП в напряжение источника питания в данном случае 3.3В
            float32_t u = car_sens_meas[channel].filtered * 3.3 / 4096;

            // ", EMA Q16.16: " _F3
            sprintf(msg, "EMA: %5lu, Raw value: %5lu, Voltage: " _F1 " V",
                    car_sens_meas[channel].filtered,
                    car_sens_meas[channel].measured_value,
                    FLOAT1(u));
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

            //! вывод в teleplot значений каналов
            uart_printf(&huart3, ">adc_ch%d_V:" _F1 "\r\n", channel, FLOAT1(u));
        }
        else
        {
            sprintf(msg, "No signal");
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }

        sprintf(msg, "\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }

    sprintf(msg, "=====================\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}
