/* Includes ------------------------------------------------------------------*/
#include <main_export.h>
#include <local_misc.h>
#include "adc.h"
#include <emb_float.h>
#include <uart_printf.h>
#include <EMA.h>

// Это пространство или слой программы, в котором уже можно и нужно использовать возможности С++.
// Здесь будут только вставки на С

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void process_ai_channel_data(uint8_t channel);
static void process_ai_measure_data(void);

// Здесь код на С не стоит уже активно использовать, следут писать на С++
// todo вынести
float ema(float measured_value, float prev_ema, float alpha);

extern "C"
{
    /**
     * @brief
     * @retval None
     */
    // Точка входа, в этой функции исполняется код зазачи в потоке операционной системы
    void appxx_car_sens_meas(void)
    {
        // АЦП работает так(очень упрощённо): поступающий сигнал сначала заряжает внутренний конденсатор,
        // а потом происходит измерение напряжения в этом конденсаторе.
        // Соответственно у поступающего сигнала должен быть большой ток чтоб быстро зарядить конденсатор.
        // Если внешний сигнал слабенький, то нужно увеличить Sampling Time.

        // Время затрачиваемое на преобразование(опрос) одного канала можно посчитать так :
        // 1.5 такта(заряд конденсатора) + 12.5 обязательных тактов(преобразование сигнала) = 14 тактов.При условии,
        // что АЦП тактируется от 12МГц, тогда 14 тактов выполнятся за одну с хвостиком микросекунду.

        // Алгоритм будет следующим: результат работы АЦП будет копироваться в массив с помощью DMA.
        // После заполнения массива будет происходить прерывание от DMA и вызывать колбек,
        // в колбеке работа с АЦП будет останавливаться, выводиться результат в UART и запускаться снова.

        // каждое измерение будет заноситься в циклический буфер
        // на основе значений в буфере будет вычисляться среднее
        // полученное значение будет вносится как значение измерямого параметра

        //* что-то по калибровке https://stackoverflow.com/questions/58328342/calibrating-stm32-adc-vrefint
        // Ещё калибровке
        // Запускайте последовательно оба типа калибровки.
        // Значения сохраняются в CALFACT_S и CALFACT_D соответственно для обычных и диф.каналов.
        // При работе с каким типом канала работаете тот коэффициент и используется.
        // * DMA example https://microtechnics.ru/stm32-adc-aczp-i-dma-obzor-nastrojka-i-primer-proekta/

        // DMA mode IO operation
        {
            // • Start the ADC peripheral using HAL_ADC_Start_DMA(), at this stage the user specify the length of data to be transferred at each end of conversion
            HAL_ADC_Start_DMA(&hadc1, (uint32_t *)dma_adc_buffer, NUM_AI_CHANNELS);
            // • At The end of data transfer by HAL_ADC_ConvCpltCallback() function is executed and user can add his own code by customization of function pointer HAL_ADC_ConvCpltCallback
            // • In case of transfer Error, HAL_ADC_ErrorCallback() function is executed and user can add his own code by customization of function pointer HAL_ADC_ErrorCallback
            // • Stop the ADC peripheral using HAL_ADC_Stop_DMA()
            // • Process measured channel data
            process_ai_measure_data();
        }
    }
} // end extern "C"

/* Task code -----------------------------------------------------------------*/

/* Private function  ---------------------------------------------------------*/

/**
 * @brief
 * @retval None
 */
// Обработка захваченных данных
void process_ai_measure_data(void)
{
    // Обработка данных для каждого канала
    for (int channel = 0; channel < NUM_AI_CHANNELS; channel++)
    {
        if (car_sens_meas[channel].data_ready_)
        {
            // Сброс флага канала
            car_sens_meas[channel].data_ready_ = 0;

            // Обработка захваченных данных канала
            process_ai_channel_data(channel);
        }
    }
}

/**
 * @brief
 * @retval None
 */
// Обработка данных конкретного канала
void process_ai_channel_data(uint8_t channel)
{
    // • EMA Filtering
    uint32_t filtered = (uint32_t)ema_simple(car_sens_meas[channel].measured_value,
                                             car_sens_meas[channel].filtered, car_sens_meas[channel].alpha);

    // Вместо прямого присваивания, помещаем в развязывающий буфер для неблокирующего доступа
    lf_circular_buf_put(car_sens_meas[channel].value_handle_, filtered);
    car_sens_meas[channel].filtered = filtered;
}
