/* Includes ------------------------------------------------------------------*/
#include <main_export.h>
#include <local_misc.h>

// Это пространство или слой программы, в котором уже можно и нужно использовать возможности С++.
// Здесь будут только вставки на С

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

// Здесь код на С не стоит уже активно использовать, следут писать на С++
static void process_pi_capture_data(void);
static void process_pi_channel_data(uint8_t channel);
static void calculate_duty_cycle(void);

extern "C"
{
    /**
     * @brief
     * @retval None
     */
    // Точка входа, в этой функции исполняется код зазачи в потоке операционной системы
    void appxx_shafts_meas(uint32_t channel)
    {
        // Обработка захваченных данных
        process_pi_capture_data();
    }
} // end extern "C"

/**
 * @brief
 * @retval None
 */
// Обработка захваченных данных
void process_pi_capture_data(void)
{
    // Вычисление периода между двумя фронтами
    // DMA записывает значения счетчика в массив dma_capture_buffer

    // Обработка данных для каждого канала
    for (int channel = 0; channel < NUM_PI_CHANNELS; channel++)
    {
        if (shafts_meas[channel].data_ready_)
        {
            // Сброс флага канала
            shafts_meas[channel].data_ready_ = 0;

            // Обработка захваченных данных канала
            process_pi_channel_data(channel);
        }
    }
}

/**
 * @brief
 * @retval None
 */
// Обработка данных конкретного канала
void process_pi_channel_data(uint8_t channel)
{
    //* Общий и правильный кмк способ расчёта частоты без привязки к константам
    // Вычисление периода между двумя фронтами
    // DMA записывает значения счетчика в массив dma_capture_buffer
    // if (captures[1] >= captures[0])
    //  period = captures[1] - captures[0]; // Нормальный случай (без переполнения)
    // else
    //  period = (htim3.Instance->ARR - captures[0]) + captures[1]; // Случай с переполнением счетчика
    // clk = HAL_RCC_GetPCLK1Freq() / (htim3.Instance->PSC + 1); // HAL_RCC_GetPCLK1Freq() Важно указывать правильно источник тактирования
    // frequency = (float)clk / period;
    // sprintf(msg, "Input frequency: %. 3f\r\n", frequency);
    // HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

    // * Эта часть перенесена в callback захвата
    // uint32_t capture1 = 0;
    // uint32_t capture2 = 0;

    // // выбираем два разных значения чтобы вычисления были корректны
    // do
    // {
    //     array_min_safe(dma_capture_buffer[channel], 2, &capture1);
    // } while (capture1 == 0);
    // do
    // {
    //     array_max_safe(dma_capture_buffer[channel], 2, &capture2);
    // } while (capture2 == capture1 || capture2 == 0);
    // // capture1 = dma_capture_buffer[channel][0];
    // // capture2 = dma_capture_buffer[channel][1];

    // uint32_t measured_period_us = 0;

    // if (capture2 > capture1)
    // {
    //     // Нормальный случай (без переполнения)
    //     measured_period_us = capture2 - capture1;
    // }
    // else
    // {
    //     // Случай с переполнением счетчика
    //     measured_period_us = (0xFFFF - capture1) + capture2 + 1; // +1 защита от 0 заначения
    // }

    // // Поместить текущее измерение в измерительный буфер
    // circular_buf_put(shafts_meas[channel].raw_period_handle_, (circular_buf_data_t)measured_period_us);

    // * Анализ буфера захвата
    // Фильтрация по взвшенному среднему(Варинты + среднее арифметическое)
    uint32_t period_us_intri = (uint32_t)circular_buf_mean_weighted_intrinsic(shafts_meas[channel].raw_period_handle_);
    // uint32_t period_us_cmsis = (uint32_t)circular_buf_mean_weighted_cmsis(shafts_meas[channel].raw_period_handle_);
    // uint32_t period_us_avg = (uint32_t)circular_buf_mean_arithmetic_f32(shafts_meas[channel].raw_period_handle_);

    // * Способ 1: Мьютексы (самый надежный)
    // Объявляется глобальный параметр tachoN2
    // По готовности нового измерения вызыватся метод tachoN2.Update(),
    // в котором производится копирование данных из структуры измерений *_meas
    // в приватные свойсва(переменные) обхъекта класса
    // Далеее объектом можно пользоваться, хотя блокировка должна быть на чтение и назапись
    // if (xSemaphoreTake(sensor1_mutex, portMAX_DELAY) == pdTRUE)
    // {
    //     // Копирование данных в глобальную структуру
    //     memcpy(&sensor1_data, &local_data, sizeof(SensorData_t));
    //     // Освобождение мьютекса
    //     xSemaphoreGive(sensor1_mutex);
    // }

    // * Способ 2: lock-free multi-producer/multi-consumer
    // Или можно сделать дополнение circular_buf в том же стиле для lock-free multi-producer/multi-consumer
    // Будет Полная lock-free реализация - нет мьютексов, нет блокировок
    // Подсчитанный и отфильтрованный из циклического буфера период
    // помещается в lock-free буфер
    // В любой момент объект класса параметра читает lock-free буфер
    // Нет мьютексов, нет блокировок
    // Допустима потеря данных измерения(они будут заменены более свежими)
    // lock-free буфер - это 2 значения
    // Это обычная переменная, но которая работает по схеме green/blue одна в работе другая заменяется
    // потом они меняются. Цена блокировки - удвоенный расход памяти

    // вместо прямого присваивания, помещаем в развязывающий буфер для неблокирующего доступа
    lf_circular_buf_put(shafts_meas[channel].period_handle_, period_us_intri);

    // ! Эта часть уже будет в определении параметра
    {
        shafts_meas[channel].period_us = period_us_intri; // ! старый вариант общая переменная без защит

        // Получить частоту тактирования
        float clk = HAL_RCC_GetPCLK2Freq() / (htim3.Instance->PSC + 1);

        // Вычисление частоты (1 МГц = 1 тик/мкс)
        float frequency = (float)clk / (float)shafts_meas[channel].period_us;

        shafts_meas[channel].frequency_hz = frequency;

        // ToDo:
        // Проверка на выбросы (например, шум)
        // if (shafts_meas[channel].period_us < 10 || shafts_meas[channel].period_us > 100000)
        // {
        //     // Отбрасываем некорректное измерение
        //     shafts_meas[channel].period_us = 0;
        //     shafts_meas[channel].frequency_hz = 0.0f;
        // }
    }

    // TODO: рекомендации
    // Аппаратные улучшения настройки таймера
    // 1. Увеличить частоту таймера (если возможно)
    //    Например, с 1 МГц до 10 МГц для большей точности
}

/**
 * @brief
 * @retval None
 */
// Вычисление скважности (для пар каналов) //todo: not uset now
void calculate_duty_cycle(void)
{
    // Канал 1 (фронт) и Канал 2 (спад) - пара
    if (shafts_meas[0].period_us > 0 && shafts_meas[1].period_us > 0)
    {
        // Вычисление времени высокого уровня
        uint32_t high_time = shafts_meas[1].period_us - shafts_meas[0].rise_time;
        shafts_meas[0].duty_cycle = (high_time * 100) / shafts_meas[0].period_us;
    }

    // Канал 3 (фронт) и Канал 4 (спад) - пара
    if (shafts_meas[2].period_us > 0 && shafts_meas[3].period_us > 0)
    {
        uint32_t high_time = shafts_meas[3].period_us - shafts_meas[2].rise_time;
        shafts_meas[1].duty_cycle = (high_time * 100) / shafts_meas[2].period_us;
    }
}
