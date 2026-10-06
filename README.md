# 🏗️ Архитектура проекта

> ⚠️ Перед генерациeй CubeMX сделайте backup commit! Переименовывать main.c в main.cpp и обратно не нужно.

## Общая идея

```text
------------------------------------------------------------
APP lyaer C++ code без HAL, CMSIS, Arduino.
            ------------------------------------------------
            Logic (service/usecase)
            ------------------------------------------------
            Repostory C++ code wrapped in C functions
------------------------------------------------------------
FreeRTOS layer C code calls C++ code wrapped in C functions
            Measures storages (intermidate buffers)
            ------------------------------------------------
            Drivers
------------------------------------------------------------
Drivers lyaer C code HAL, CMSIS, Arduino.

------------------------------------------------------------
Hardware layer C, ASM code HAL, LL bare registries

------------------------------------------------------------
External priferial devices layer

/ / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
```

## 1. **Платформонезависимый слой (Core Logic)**

- **Transmission Control Core**  
  - Принимает: нормализованные параметры (`throttle_pct`, `rpm`, `speed_kph`, `atf_temp_c`, `gear_ratio`, `slippage_pct` и т.д.)  
  - Выдаёт: абстрактные команды (`shift_up`, `shift_down`, `lock_torque_converter`, `target_pressure_kPa`)  
  - Не знает ничего о железе, датчиках, шинах.

- **Monitoring & Diagnostics**  
  - Формирует структуру данных для логгирования/отладки (`TelemetryPacket`)  
  - Поддерживает runtime-конфигурацию (через `ConfigManager`)

> ✅ Всё это — обычные `.cpp/.hpp` файлы без HAL, CMSIS, Arduino.
> ✅ Все абстрактные параметры предоставляют **нормализованные значения** (0–100%, °C, kPa, об/мин и т.д.) в едином формате.

---

## 2. **Платформозависимый слой (Hardware Abstraction Layer — HAL)**

- **Repositories** предоставляют доступ к метрикам через структуры *_meas типа *_measure_t
  - ADC + DMA → TPS, температура, давление  
  - TIM Input Capture → N2/N3, RPM  

- **Sensor Drivers**  
  - ADC + DMA → TPS, температура, давление  
  - TIM Input Capture → N2/N3, RPM  
  - Frequency-to-voltage / pulse counting → скорость  
  - CAN (если понадобится) → приборка, руль

- **Actuator Drivers**  
  - TIM PWM → соленоиды, спидометр, топливный насос  
  - GPIO → реле, horn

- **Communication Interfaces**  
  - SPI → OLED  
  - **USART или SPI** → связь с ESP32 (см. ниже)

> ✅ Все драйверы предоставляют **нормализованные значения** метрики (0–100%, °C, kPa, об/мин и т.д.) в едином формате. Где это возможно сразу.

---

## 3. **FreeRTOS Task Layout (приоритеты условны)**

| Задача | Приоритет | Описание |
|-------|----------|--------|
| `Task_Sensors` | High | Чтение всех датчиков → обновление глобального `SensorState` (защищено через `xSemaphore` или lock-free если atomic) |
| `Task_Transmission` | High | Основная логика АКПП на основе `SensorState` → обновление `ControlOutput` |
| `Task_Actuators` | Medium-High | Применение `ControlOutput` к PWM/GPIO |
| `Task_Monitoring` | Medium | Формирование телеметрии, логгирование, обработка конфигурации |
| `Task_ESP32_Comm` | Medium | Обмен данными с ESP32 (UART/SPI) |
| `Task_Display` | Low | Обновление OLED |

> ⚠️ Жёсткие временные требования (например, синхронизация с N2/N3) — выполняются в прерываниях или высокоприоритетных задачах.

---

В каталоге `appxx` располагаются .cpp файлы и высокоуровневая логика приложения.

Пример для связи Си и С++ кода. Данные функции вызываются в потоках FreeRTOS и реализуют функцию репозитория для доступа к измерениям и исполнительным устройствам из абстрактной логики приложения на С++.
Это граница раздела кода на Си и С++.

Файлы `include\c_wrapper4cxx_code_exec.hpp` и `src\appxx\c_wrapper4cxx_code_exec.cpp`.

```cpp
/**
 * Example how to call C++ code from C layer (from FreeRTOS thread here)
 */
#include <c_wrapper4cxx_code_exec.hpp>
#include "usart.h"
#include <xx_types.hpp>
#include <stdio.h>
#include <string.h>
using namespace app;

// Это пространство или слой программы, в котором уже можно и нужно использовать возможности С++.
app::Time t(12, 24, 45);

// Можно вызывать C++ код из C, обернув его в функции в стиле C.
// Для этого нужно использовать extern "C" в C++ коде.
// Вот как это сделать:
// Функции в стиле C с extern "C"
extern "C"
{
    void c_wrapper4cxx_code_exec(void)
    {
        char msg[64] = {0};
        sprintf(msg, "\r\n-Time-: %d H: %d M : %d M\r\n", t.GetHours(), t.GetMinutes(), t.GetSeconds());
        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }
}
// Далее вызываем эту функцию в Си коде.
// При необходимости в неё можно передать аргументы для взаимодействия с С++ кодом.
```

Файл `include\mainxx.hpp` нужен для связи объявлений на Си и кода на С++. Инаначе ошибка компиляции. Если не переименовавывать main.c в main.cpp, то этот файл не нужен.

Содержит объявление одной лишь функции из freertos.c. Без этого main.cpp не компилируется. 



```cpp
#ifndef MAINXX_HPP
#define MAINXX_HPP

#ifdef __cplusplus
extern "C"
{
#endif
    // TODO: сюда добавлять функции которые не видятся при компиляции при переходе от main.c к main.cpp
    void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
#ifdef __cplusplus
}
#endif

#endif /* MAINXX_HPP */
```

Сейчас можно ничего не переименовывать и писать в разных слоях на Си и на С++.
Нижний уровень. Всё что ниже и на уровне ОС(FreeRTOS) и её потоков(TaskThreads). Использовать Си.
Вся абстрактная логика может быть написано целиком на С++.
Связывание потоков исполнения и логики происходит файлах каталога `appxx` где C++ код обарачивается в Си функции.
Таким образом происходит объединение.

Файл `include\xx_types.hpp` содержит определения типов приложения на С++. Можно в подобном стиле создавать и другие файлы для удобства струкруры.

Файлы `main_types.h`, `main_export.h` нужны чтобы отвязаться от генерируемых файлов CubeMX. Это по существу тот же `main.h`
