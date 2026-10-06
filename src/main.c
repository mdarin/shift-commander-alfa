/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main μC program body
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <main_types.h>
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

/* USER CODE BEGIN PV */
volatile uint8_t blink_flag = 0;                                                     // Флаг для основной программы
static uint32_t pi_cbuf_storage_ch_[NUM_PI_CHANNELS][PI_CIRCULAR_BUFFER_SIZE] = {0}; // PI circular buffer back storage
static uint32_t pi_lf_cbuf_storage_ch_[NUM_PI_CHANNELS][2] = {0};                    // PI lock-free circular buffer back storage
static uint32_t ai_lf_cbuf_storage_ch_[NUM_AI_CHANNELS][2] = {0};                    // AI lock-free circular buffer back storage
uint32_t dma_adc_buffer[NUM_AI_CHANNELS] = {0};                                      // Буфер для сырых данных из регистра АЦП для DMA
uint32_t dma_capture_buffer[NUM_PI_CHANNELS][SAMPLES_PER_PI_CHANNEL] = {0};          // Двухмерный буфер [канал][семпл] для DMA
pulse_measurement_t shafts_meas[NUM_PI_CHANNELS] = {0};                              // Структура для хранения измерений датчиков оборотов валов(shafts) PI channels
analog_measurement_t car_sens_meas[NUM_AI_CHANNELS] = {0};                           // Car sensors measurements for AI channels
digital_measurement_t pnps_meas = {0};                                               // PNPS Position switch state DI // ! deprecated
digital_measurement_t car_switchs_meass /*[NUM_DI_CHANNELS]*/ = {0};                 // Car switches and buttons states for DI channels

// * все измерения хранятся в соответвующих структурах с постфиксом *_meas
// * эти измерения используются в расчёте значений параметров(класс параметр со свойствами)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void shafts_meas_init(void);
void car_sens_meas_init(void);
uint32_t calculate_period(uint32_t, uint32_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_TIM9_Init();
  MX_TIM12_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* Initialize all configured repository storages(measurements) */
  shafts_meas_init();
  car_sens_meas_init();

  /* Initializers */
  // TODO: здесь только инициализации на манер тех, что генерирует CubeMX для перифирии

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // ! Данный код не вызывается μC
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
 * @brief  This function is executed in case of capture done.
 * @retval None
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  // Можно обрабатывать половину данных, пока заполняется вторая половина
  // индикация (например, мигание светодиода)
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

  // Обработчик прерывания от DMA
  if (htim->Instance == TIM3)
  {
    // Определяем, какой канал вызвал прерывание
    // Установка флага для основной программы
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
      uint32_t capture1 = dma_capture_buffer[0][2];
      uint32_t capture2 = dma_capture_buffer[0][3];
      uint32_t period_us = calculate_period(capture1, capture2);

      // Поместить текущее измерение в измерительный буфер
      circular_buf_put(shafts_meas[0].raw_period_handle_, (circular_buf_data_t)period_us);

      // уведомить о готовности данных
      shafts_meas[0].data_ready_ = 1;
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
      uint32_t capture1 = dma_capture_buffer[1][2];
      uint32_t capture2 = dma_capture_buffer[1][3];
      uint32_t period_us = calculate_period(capture1, capture2);

      circular_buf_put(shafts_meas[1].raw_period_handle_, (circular_buf_data_t)period_us);

      shafts_meas[1].data_ready_ = 1;
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
    {
      uint32_t capture1 = dma_capture_buffer[2][2];
      uint32_t capture2 = dma_capture_buffer[2][3];
      uint32_t period_us = calculate_period(capture1, capture2);

      circular_buf_put(shafts_meas[2].raw_period_handle_, (circular_buf_data_t)period_us);

      shafts_meas[2].data_ready_ = 1;
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
    {
      uint32_t capture1 = dma_capture_buffer[3][2];
      uint32_t capture2 = dma_capture_buffer[3][3];
      uint32_t period_us = calculate_period(capture1, capture2);

      circular_buf_put(shafts_meas[3].raw_period_handle_, (circular_buf_data_t)period_us);

      shafts_meas[3].data_ready_ = 1;
    }
  }
}

/**
 * @brief  This function is executed in case of period elapsed.
 * @retval None
 */
// Колбэки при заполнении половины/полного буфера
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim)
{
  // Можно обрабатывать половину данных, пока заполняется вторая половина
  // индикация (например, мигание светодиода)
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

  // Обработчик прерывания от DMA
  if (htim->Instance == TIM3)
  {
    // Определяем, какой канал вызвал прерывание
    // Установка флага для основной программы
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
      uint32_t capture1 = dma_capture_buffer[0][0];
      uint32_t capture2 = dma_capture_buffer[0][1];
      uint32_t period_us = calculate_period(capture1, capture2);

      // Поместить текущее измерение в измерительный буфер
      circular_buf_put(shafts_meas[0].raw_period_handle_, (circular_buf_data_t)period_us);

      // уведомить о готовности данных
      shafts_meas[0].data_ready_ = 1;
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
      uint32_t capture1 = dma_capture_buffer[1][0];
      uint32_t capture2 = dma_capture_buffer[1][1];
      uint32_t period_us = calculate_period(capture1, capture2);

      circular_buf_put(shafts_meas[1].raw_period_handle_, (circular_buf_data_t)period_us);

      shafts_meas[1].data_ready_ = 1;
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
    {
      uint32_t capture1 = dma_capture_buffer[2][0];
      uint32_t capture2 = dma_capture_buffer[2][1];
      uint32_t period_us = calculate_period(capture1, capture2);

      circular_buf_put(shafts_meas[2].raw_period_handle_, (circular_buf_data_t)period_us);

      shafts_meas[2].data_ready_ = 1;
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
    {
      uint32_t capture1 = dma_capture_buffer[3][0];
      uint32_t capture2 = dma_capture_buffer[3][1];
      uint32_t period_us = calculate_period(capture1, capture2);

      circular_buf_put(shafts_meas[3].raw_period_handle_, (circular_buf_data_t)period_us);

      shafts_meas[3].data_ready_ = 1;
    }
  }
}

/**
 * Расчет периода между двумя захватами таймера
 * @param first_capture  - значение при первом захвате (более раннее)
 * @param second_capture - значение при втором захвате (более позднее)
 * @return период в тиках (1..0xFFFFFFFF)
 */
uint32_t calculate_period(uint32_t capture1, uint32_t capture2)
{
  uint32_t period_us = 0;

  if (capture2 > capture1)
  {
    // Нормальный случай (без переполнения)
    period_us = capture2 - capture1;
  }
  else
  {
    // Случай с переполнением счетчика
    period_us = (0xFFFF - capture1) + capture2; //+ 1;
  }

  return (period_us < 1) ? 1 : period_us;
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM7 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  /* Обработчик прерывания TIM6 */
  if (htim->Instance == TIM6)
  {
    blink_flag = 1; // Установка флага для основной программы
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1)
  {
    // fetch ADC measured values to the measurement storage
    for (uint8_t i = 0; i < NUM_AI_CHANNELS; i++)
    {
      car_sens_meas[i].measured_value = dma_adc_buffer[i];
      car_sens_meas[i].data_ready_ = 1;
    }
  }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  // todo handle adc error
  // __FILE__ __LINE__
}

/**
 * @brief  This function is an example of measurements initialization
 * @retval None
 */
void shafts_meas_init(void)
{
  // * Для каждого вида измерений необходимо своё хранилищел
  // к примеру:
  // shafts_meas - это измерения оборотов валов
  // car_sens_meas - это измерения датчиков температуры, давления, напряжения борт сети и пр.
  // sel_meas - это определение положения селектора АКПП
  // В такие хранилища помещаются предобработанные(отфильтрованные данные)
  // �?з такого хранилища получает данные объект класса соответвующего параметра
  // Эта связка формирует _репозиторий параметра_
  // Далее, в нутри класса измерение приводится к нужному виду параметра.
  // Таким образом параметр обретает значение

  // Получить частоту тактирования
  float clk = HAL_RCC_GetPCLK2Freq() / (htim3.Instance->PSC + 1);

  for (int i = 0; i < NUM_PI_CHANNELS; i++)
  {
    shafts_meas[i].data_ready_ = 0;
    // shafts_meas[i].last_value = 0;      // deprecated
    // shafts_meas[i].period_us = 0;       // deprecated заменена на lock-free
    // shafts_meas[i].frequency_hz = 0.0f; // deprecated
    // shafts_meas[i].duty_cycle = 0;      // deprecated
    shafts_meas[i].raw_period_handle_ = circular_buf_init(pi_cbuf_storage_ch_[i], PI_CIRCULAR_BUFFER_SIZE);
    shafts_meas[i].period_handle_ = lf_circular_buf_init(pi_lf_cbuf_storage_ch_[i], 2); // todo change to config param(2 is a good choise)
    shafts_meas[i].clk_ = clk;                                                          // вносим это значение один раз чтобы не считать каждый раз
  }
}

/**
 * @brief  This function is an example of measurements initialization
 * @retval None
 */
void car_sens_meas_init(void)
{
  for (int i = 0; i < NUM_AI_CHANNELS; i++)
  {
    // Configure for the selected ADC regular channel
    car_sens_meas[i].filtered = 0; // deprecated
    car_sens_meas[i].measured_value = 0;
    car_sens_meas[i].alpha = 0.3f;                                                       // Чем меньше alpha, тем больше сглаживание
    car_sens_meas[i].value_handle_ = lf_circular_buf_init(ai_lf_cbuf_storage_ch_[i], 2); // todo change to config param(2 is a good choise)
  }

  // Выбор alpha:
  // - alpha = 0.1 ≈ 19 периодов (сильное сглаживание)
  // - alpha = 0.2 ≈ 9 периодов
  // - alpha = 0.3 ≈ 5.7 периодов
  // - alpha = 0.5 ≈ 3 периода (умеренное сглаживание)
  // - alpha = 0.8 ≈ 1.5 периодов (слабое сглаживание)
  // - alpha = 1.0 = нет сглаживания (просто текущее значение)
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    // Error indication //TODO: Diagnostic
    // HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    HAL_Delay(50);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
