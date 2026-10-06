/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <main_export.h>
#include <local_misc.h>
#include "usart.h"
#include "tim.h"
#include "adc.h"
#include <arm_math.h> // CMSIS-DSP библиотека от ARM
#include <c_wrapper4cxx_code_exec.hpp>
#include <appxx_shafts_meas.hpp>
#include <appxx_car_sens_meas.hpp>
#include <appxx_blinker.hpp>
#include <uart_printf.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow4,
};
/* Definitions for blinkerTask */
osThreadId_t blinkerTaskHandle;
const osThreadAttr_t blinkerTask_attributes = {
    .name = "blinkerTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow5,
};
/* Definitions for shaftsTask */
osThreadId_t shaftsTaskHandle;
const osThreadAttr_t shaftsTask_attributes = {
    .name = "shaftsTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for batteryTask */
osThreadId_t batteryTaskHandle;
const osThreadAttr_t batteryTask_attributes = {
    .name = "batteryTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for pi1Task */
osThreadId_t pi1TaskHandle;
const osThreadAttr_t pi1Task_attributes = {
    .name = "pi1Task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for aiTask */
osThreadId_t aiTaskHandle;
const osThreadAttr_t aiTask_attributes = {
    .name = "aiTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for diTask */
osThreadId_t diTaskHandle;
const osThreadAttr_t diTask_attributes = {
    .name = "diTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for doTask */
osThreadId_t doTaskHandle;
const osThreadAttr_t doTask_attributes = {
    .name = "doTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for poTask */
osThreadId_t poTaskHandle;
const osThreadAttr_t poTask_attributes = {
    .name = "poTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for pi2Task */
osThreadId_t pi2TaskHandle;
const osThreadAttr_t pi2Task_attributes = {
    .name = "pi2Task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for pi3Task */
osThreadId_t pi3TaskHandle;
const osThreadAttr_t pi3Task_attributes = {
    .name = "pi3Task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for pi4Task */
osThreadId_t pi4TaskHandle;
const osThreadAttr_t pi4Task_attributes = {
    .name = "pi4Task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartBlinkerTask(void *argument);
void StartShaftsMeasTask(void *argument);
void StartBattVoltMeasTask(void *argument);
void StartPulseInputCh1Task(void *argument);
void StartAnalogInputTask(void *argument);
void StartDiscreteInputTask(void *argument);
void StartDiscreteOutputTask(void *argument);
void StartPulseOutputTask(void *argument);
void StartPulseInputCh2Task(void *argument);
void StartPulseInputCh3Task(void *argument);
void StartPulseInputCh4Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  blinkerTaskHandle = osThreadNew(StartBlinkerTask, NULL, &blinkerTask_attributes);
  shaftsTaskHandle = osThreadNew(StartShaftsMeasTask, NULL, &shaftsTask_attributes);
  batteryTaskHandle = osThreadNew(StartBattVoltMeasTask, NULL, &batteryTask_attributes);
  doTaskHandle = osThreadNew(StartDiscreteOutputTask, NULL, &doTask_attributes);
  diTaskHandle = osThreadNew(StartDiscreteInputTask, NULL, &doTask_attributes);

  // TODO: Add your threads here

  /* USER CODE END Init */
  /* USER CODE BEGIN Header */
  /**
   ******************************************************************************
   * File Name          : freertos.c
   * Description        : Code for freertos applications
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

  /**
   * @}
   */

  /**
   * @}
   */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  uint8_t i = 0;
  uint8_t duty = 5;
  uint8_t up = 1;
  bool started = false;

  // Setup PWM 30% duty+
  set_PWM_duty16(&htim9, TIM_CHANNEL_1, 5);
  set_PWM_duty16(&htim9, TIM_CHANNEL_2, 10);
  set_PWM_duty16(&htim12, TIM_CHANNEL_1, 5);
  set_PWM_duty16(&htim12, TIM_CHANNEL_2, 5);

  if (HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1) != HAL_OK || HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2) != HAL_OK)
  {
    // MySerial.println("ERR: TIM9 PWM start failed");
    Error_Handler();
  }

  // Запуск таймера T12
  if (HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1) != HAL_OK || HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2) != HAL_OK)
  {
    // MySerial.println("ERR: TIM12 PWM start failed");
    Error_Handler();
  }

  /* Infinite loop */
  for (;;)
  {
    if (++i >= 100)
    {
      if (duty < 0 || duty >= 100)
      {
        up = !up;
        started = true;
      }

      if (up)
      {
        duty += 5;
      }
      else
      {
        duty -= 5;
      }

      set_PWM_duty16(&htim9, TIM_CHANNEL_1, duty);
      set_PWM_duty16(&htim9, TIM_CHANNEL_2, duty);
      if (90 >= duty && duty >= 15)
      {
        set_PWM_duty16(&htim12, TIM_CHANNEL_1, duty);
      }
      set_PWM_duty16(&htim12, TIM_CHANNEL_2, duty);

      //* пример функции c_wrapper4cxx_code_exec связи С кода базы и С++ кода приложения
      c_wrapper4cxx_code_exec();

      i = 0;
    }

    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartBlinkerTask */
/**
 * @brief Function implementing the blinkerTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartBlinkerTask */
void StartBlinkerTask(void *argument)
{
  /* USER CODE BEGIN StartBlinkerTask */
  uint32_t counter = 0;

  // Запуск таймера T6
  if (HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
  {
    Error_Handler();
  }

  /* Infinite loop */
  for (;;)
  {
    // Проверка флага в основном цикле
    if (blink_flag)
    {
      blink_flag = 0; // Сброс флага
      HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    }
    // TODO: проверить и использовать атомарные операции, пример:
    // 1. Проверка флага завершения DMA
    // if (__atomic_exchange_n(&dma_complete, 0, __ATOMIC_SEQ_CST))
    // {
    //   // Обработка
    // }

    // Код задачи исполняемый в потоке операционной системы
    appxx_blinker(counter);

    counter++;

    osDelay(50);
    // HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
  }
  /* USER CODE END StartBlinkerTask */
}

/* USER CODE BEGIN Header_StartShaftsMeasTask */
/**
 * @brief Function implementing the shaftsTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartShaftsMeasTask */
void StartShaftsMeasTask(void *argument)
{
  /* USER CODE BEGIN StartShaftsMeasTask */

  // Запуск захвата T3 с DMA для всех каналов
  if (HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)dma_capture_buffer[0], SAMPLES_PER_PI_CHANNEL) != HAL_OK ||
      HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)dma_capture_buffer[1], SAMPLES_PER_PI_CHANNEL) != HAL_OK ||
      HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_3, (uint32_t *)dma_capture_buffer[2], SAMPLES_PER_PI_CHANNEL) != HAL_OK ||
      HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_4, (uint32_t *)dma_capture_buffer[3], SAMPLES_PER_PI_CHANNEL) != HAL_OK)
  {
    Error_Handler();
  }

  // uint32_t channel;

  /* Infinite loop */
  for (;;)
  {
    // * Уровень планирования времени (Scheduling)
    // * здесь можно ставить мутекс или семафор для блокировки данных захвата на время измерения
    // * отправлять в очередь результаты и пр.

    // Получаем сигнал без данных
    // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Получаем сигнал и данные(номер канала)
    // if (xTaskNotifyWait(0, 0, &channel, portMAX_DELAY) == pdPASS)
    // {
    // После получения сигнала выполняем обработку
    // appxx_shafts_meas(channel);
    // }
    // ! osDelay(1); не нужен при xTaskNotifyWait

    // * Это функция потока - точка входа в код приложения исполняемого в обёртке ресурсов - потоке операционной системы.
    // * Код прилоожения пишется на С++ и эта функция является переходом от С++ к С коду
    // * НЕ СТО�?Т СМЕШ�?ВАТЬ ЛОГ�?КУ ПР�?ЛОЖЕН�?Я �? ПЛАН�?РОВАН�?Е ВРЕМЕН�? ВЫОЛНЕН�?Я(Scheduling)
    // * Это разные слои и у них различное назначение. Межпроцессные коммуникации на уровне потоков ОС
    // * Полученные данные передаются в функцию потока аргументами.
    appxx_shafts_meas(0);

    osDelay(1);
  }
  /* USER CODE END StartShaftsMeasTask */
}

/* USER CODE BEGIN Header_StartBattVoltMeasTask */
/**
 * @brief Function implementing the batteryTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartBattVoltMeasTask */
void StartBattVoltMeasTask(void *argument)
{
  /* USER CODE BEGIN StartBattVoltMeasTask */

  // ✅ Пример измерения времени выполнения операции с точностью ~1 мс :
  // Пример функции, время выполнения которой хотим измерить
  //     void my_operation(void)
  // {
  // имитация полезной работы
  //   vTaskDelay(pdMS_TO_TICKS(10)); // Задержка 10 мс
  // }
  // void measure_execution_time(void)
  // {
  //   TickType_t start, end;
  //   uint32_t elapsed_ms;
  //   start = xTaskGetTickCount(); // Получаем текущее количество тиков
  //   my_operation(); // Выполняем операцию
  //   end = xTaskGetTickCount(); // Снова получаем тики
  //   elapsed_ms = (end - start) * portTICK_PERIOD_MS; // Переводим в миллисекунды
  //   printf("Operation took %lu ms\n", elapsed_ms);
  // }

  // * Перед бесконечным циклом добавляем калибровку АЦП
  // HAL_ADC_Calibration_Start(&hadc3);
  // TODO: надо разобраться с калибровкой, так не работает функция!
  // if (HAL_ADCEx_Calibration_Start(&hadc3) != HAL_OK)
  // {
  //   Error_Handler();
  // }
  // https://stm32l4-libraries-documentation.readthedocs.io/en/latest/group__ADCEx__Exported__Functions__Group1.html#ga12d140e07af14ab117239e9d0b4a6545
  // HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED); // stm32f4xx_hal_adc_ex.h

  /* Infinite loop */
  for (;;)
  {
    appxx_car_sens_meas();

    // Уровень операционной системы, здесь тайм-менеджмент, обмен сообщениямими, синхронизация и пр.
    // Это не место для управляющей логики приложения!

    // здесь можно ставить мутекс или семафор для блокировки данных АЦП на время измерения
    // отправлять в очередь результаты и пр.
    osDelay(1000);
  }
  /* USER CODE END StartBattVoltMeasTask */
}

/* USER CODE BEGIN Header_StartPulseInputCh1Task */
/**
 * @brief Function implementing the pi1Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartPulseInputCh1Task */
void StartPulseInputCh1Task(void *argument)
{
  /* USER CODE BEGIN StartPulseInputCh1Task */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartPulseInputCh1Task */
}

/* USER CODE BEGIN Header_StartAnalogInputTask */
/**
 * @brief Function implementing the aiTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartAnalogInputTask */
void StartAnalogInputTask(void *argument)
{
  /* USER CODE BEGIN StartAnalogInputTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartAnalogInputTask */
}

/* USER CODE BEGIN Header_StartDiscreteInputTask */
/**
 * @brief Function implementing the diTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDiscreteInputTask */
void StartDiscreteInputTask(void *argument)
{
  /* USER CODE BEGIN StartDiscreteInputTask */
  uint8_t port_value;

  /* Infinite loop */
  for (;;)
  {
    /* Чтение 8 бит с порта (PB0-PB7) */
    port_value = (uint8_t)(HAL_GPIO_ReadPin(DI13_GPIO_Port, DI14_Pin) |
                           (HAL_GPIO_ReadPin(DI13_GPIO_Port, DI13_Pin) << 1) |
                           (HAL_GPIO_ReadPin(DI13_GPIO_Port, DI12_Pin) << 2) |
                           (HAL_GPIO_ReadPin(DI13_GPIO_Port, DI11_Pin) << 3) |
                           (HAL_GPIO_ReadPin(DI13_GPIO_Port, DI10_Pin) << 4) |
                           (HAL_GPIO_ReadPin(DI13_GPIO_Port, DI9_Pin) << 5) |
                           (HAL_GPIO_ReadPin(DI13_GPIO_Port, DI8_Pin) << 6) |
                           (HAL_GPIO_ReadPin(DI13_GPIO_Port, DI3_Pin) << 7));

    /* Альтернативный способ (если пины идут подряд по номерам): */
    // port_value = (uint8_t)(GPIOB->IDR & 0xFF);

    /* Форматирование строки: "PORT: 10101010\r\n" */
    /* Отправка через USART2 */
    uart_printf(&huart2, "PORT: %02X (%c%c%c%c%c%c%c%c)\r\n",
                port_value,
                (port_value & 0x80) ? '1' : '0',
                (port_value & 0x40) ? '1' : '0',
                (port_value & 0x20) ? '1' : '0',
                (port_value & 0x10) ? '1' : '0',
                (port_value & 0x08) ? '1' : '0',
                (port_value & 0x04) ? '1' : '0',
                (port_value & 0x02) ? '1' : '0',
                (port_value & 0x01) ? '1' : '0');

    osDelay(100);
  }
  /* USER CODE END StartDiscreteInputTask */
}

/* USER CODE BEGIN Header_StartDiscreteOutputTask */
/**
 * @brief Function implementing the doTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDiscreteOutputTask */
void StartDiscreteOutputTask(void *argument)
{
  /* USER CODE BEGIN StartDiscreteOutputTask */
  HAL_GPIO_WritePin(DO12_GPIO_Port, DO12_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DO14_GPIO_Port, DO14_Pin, GPIO_PIN_SET);

  /* Infinite loop */
  for (;;)
  {

    HAL_GPIO_TogglePin(DO11_GPIO_Port, DO11_Pin);
    HAL_GPIO_TogglePin(DO12_GPIO_Port, DO12_Pin);
    HAL_GPIO_TogglePin(DO13_GPIO_Port, DO13_Pin);
    HAL_GPIO_TogglePin(DO14_GPIO_Port, DO14_Pin);

    osDelay(3000);
  }
  /* USER CODE END StartDiscreteOutputTask */
}

/* USER CODE BEGIN Header_StartPulseOutputTask */
/**
 * @brief Function implementing the poTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartPulseOutputTask */
void StartPulseOutputTask(void *argument)
{
  /* USER CODE BEGIN StartPulseOutputTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartPulseOutputTask */
}

/* USER CODE BEGIN Header_StartPulseInputCh2Task */
/**
 * @brief Function implementing the pi2Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartPulseInputCh2Task */
void StartPulseInputCh2Task(void *argument)
{
  /* USER CODE BEGIN StartPulseInputCh2Task */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartPulseInputCh2Task */
}

/* USER CODE BEGIN Header_StartPulseInputCh3Task */
/**
 * @brief Function implementing the pi3Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartPulseInputCh3Task */
void StartPulseInputCh3Task(void *argument)
{
  /* USER CODE BEGIN StartPulseInputCh3Task */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartPulseInputCh3Task */
}

/* USER CODE BEGIN Header_StartPulseInputCh4Task */
/**
 * @brief Function implementing the pi4Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartPulseInputCh4Task */
void StartPulseInputCh4Task(void *argument)
{
  /* USER CODE BEGIN StartPulseInputCh4Task */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartPulseInputCh4Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
