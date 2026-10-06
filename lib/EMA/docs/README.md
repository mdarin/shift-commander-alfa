Вот простейшая реализация EMA (Exponential Moving Average) на C:

## Базовая реализация EMA

### Вариант 1: Минималистичный (без структуры)

```c
#include <stdint.h>

// Простейшая функция EMA на каждый вызов
float ema_simple(float current_value, float previous_ema, float alpha) {
    return alpha * current_value + (1.0f - alpha) * previous_ema;
}

// Использование:
float measured_value = 10.0f;
float ema_result = 0.0f;
float alpha = 0.1f;  // Коэффициент сглаживания (0 < alpha ≤ 1)

// При первом вызове можно инициализировать первым значением
ema_result = measured_value;

// При последующих вызовах
ema_result = ema_simple(measured_value, ema_result, alpha);
```

### Вариант 2: С простой структурой

```c
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float value;    // Текущее значение EMA
    float alpha;    // Коэффициент сглаживания
    bool initialized; // Флаг инициализации
} EMA_Filter;

// Инициализация фильтра
void ema_init(EMA_Filter *filter, float alpha) {
    filter->alpha = alpha;
    filter->value = 0.0f;
    filter->initialized = false;
}

// Обновление фильтра новым измерением
float ema_update(EMA_Filter *filter, float measured_value) {
    if (!filter->initialized) {
        // При первом измерении просто запоминаем значение
        filter->value = measured_value;
        filter->initialized = true;
    } else {
        // Применяем формулу EMA
        filter->value = filter->alpha * measured_value + 
                       (1.0f - filter->alpha) * filter->value;
    }
    return filter->value;
}

// Получение текущего значения
float ema_get(EMA_Filter *filter) {
    return filter->value;
}
```

### Вариант 3: С фиксированной точкой (для встраиваемых систем)

```c
#include <stdint.h>

// Используем fixed-point арифметику (Q16.16)
typedef int32_t fixed_t;
#define FIXED_SHIFT 16
#define FLOAT_TO_FIXED(x) ((fixed_t)((x) * (1 << FIXED_SHIFT)))
#define FIXED_TO_FLOAT(x) ((float)(x) / (1 << FIXED_SHIFT))

typedef struct {
    fixed_t value;      // Текущее значение EMA в fixed-point
    fixed_t alpha;      // Коэффициент сглаживания в fixed-point
    uint8_t initialized;
} EMA_Filter_Fixed;

void ema_fixed_init(EMA_Filter_Fixed *filter, float alpha) {
    filter->alpha = FLOAT_TO_FIXED(alpha);
    filter->value = 0;
    filter->initialized = 0;
}

fixed_t ema_fixed_update(EMA_Filter_Fixed *filter, fixed_t measured_value) {
    if (!filter->initialized) {
        filter->value = measured_value;
        filter->initialized = 1;
    } else {
        // EMA формула в fixed-point арифметике
        // value = alpha * measured + (1-alpha) * value
        fixed_t term1 = (filter->alpha * measured_value) >> FIXED_SHIFT;
        fixed_t term2 = ((((1 << FIXED_SHIFT) - filter->alpha) * filter->value) >> FIXED_SHIFT);
        filter->value = term1 + term2;
    }
    return filter->value;
}
```

### Вариант 4: С расчетом alpha на основе N (количество периодов)

```c
#include <math.h>

typedef struct {
    float value;
    float alpha;
    uint32_t n;        // Количество периодов для расчета alpha
    bool initialized;
} EMA_Filter_N;

// alpha = 2/(N+1) - стандартная формула
void ema_n_init(EMA_Filter_N *filter, uint32_t n_periods) {
    filter->n = n_periods;
    filter->alpha = 2.0f / (n_periods + 1);
    filter->value = 0.0f;
    filter->initialized = false;
}

float ema_n_update(EMA_Filter_N *filter, float measured_value) {
    if (!filter->initialized) {
        filter->value = measured_value;
        filter->initialized = true;
    } else {
        filter->value = filter->alpha * measured_value + 
                       (1.0f - filter->alpha) * filter->value;
    }
    return filter->value;
}

// Изменение N в реальном времени
void ema_n_set_periods(EMA_Filter_N *filter, uint32_t n_periods) {
    filter->n = n_periods;
    filter->alpha = 2.0f / (n_periods + 1);
}
```

### Вариант 5: Самый простой (одна функция)

```c
// Самая простая реализация - все в одной функции
// measured_value - текущее измерение
// prev_ema - предыдущее значение EMA
// alpha - коэффициент (например, 0.1 для 10 периодов)
float ema(float measured_value, float prev_ema, float alpha) {
    return (alpha * measured_value) + ((1.0f - alpha) * prev_ema);
}

// Использование:
float current_ema = 0;
float alpha = 0.1f;  // Чем меньше alpha, тем больше сглаживание

// Инициализация первым значением
current_ema = get_measured_value();

// В основном цикле
while(1) {
    float measured = get_measured_value();
    current_ema = ema(measured, current_ema, alpha);
    // Используем current_ema
}
```

## Пример использования

```c
#include <stdio.h>

int main() {
    // Используем вариант 2
    EMA_Filter filter;
    ema_init(&filter, 0.1f);  // alpha = 0.1 (сильное сглаживание)
    
    // Имитация измерений с шумом
    float measurements[] = {10.0f, 10.5f, 9.8f, 11.2f, 10.1f, 9.9f};
    
    printf("Измерение\tEMA\n");
    printf("---------\t---\n");
    
    for (int i = 0; i < 6; i++) {
        float ema_value = ema_update(&filter, measurements[i]);
        printf("%.2f\t\t%.2f\n", measurements[i], ema_value);
    }
    
    return 0;
}
```

## Выбор alpha

- **alpha = 0.1** ≈ 19 периодов (сильное сглаживание)
- **alpha = 0.2** ≈ 9 периодов
- **alpha = 0.3** ≈ 5.7 периодов
- **alpha = 0.5** ≈ 3 периода (умеренное сглаживание)
- **alpha = 0.8** ≈ 1.5 периодов (слабое сглаживание)
- **alpha = 1.0** = нет сглаживания (просто текущее значение)

## Формула EMA

```
EMA_today = alpha × value_today + (1 - alpha) × EMA_yesterday
```

где:

- `alpha = 2 / (N + 1)` для N периодов
- `0 < alpha ≤ 1`

## Преимущества EMA

- Простая реализация
- Малая вычислительная сложность
- Не требует хранения истории (только предыдущее значение)
- Хорошо сглаживает шумы
- Быстрая реакция на изменения

Эта реализация идеально подходит для встраиваемых систем из-за минимальных требований к памяти и вычислениям.

## Вариант 3: С фиксированной точкой (для встраиваемых систем)

Да, абсолютно правильно! Если ваши данные гарантированно неотрицательные (например, амплитуды, мощности сигналов, расстояния, и т.д.), то использование `uint32_t` для fixed-point Q16.16 формата даст несколько преимуществ:

## Почему uint32_t лучше в вашем случае

1. **Дополнительный бит точности** - вместо знакового бита получаете ещё один бит для целой части
2. **Упрощение операций** - не нужно думать о знаке при сдвигах
3. **Больший динамический диапазон** - максимальное значение 4.29e9 вместо 2.14e9

## Модифицированный код с uint32_t

```c
#include <stdint.h>

// Используем fixed-point арифметику (UQ16.16) - беззнаковый Q16.16
typedef uint32_t ufixed_t;
#define FIXED_SHIFT 16
#define FLOAT_TO_UFIXED(x) ((ufixed_t)((x) * (1 << FIXED_SHIFT)))
#define UFIXED_TO_FLOAT(x) ((float)(x) / (1 << FIXED_SHIFT))

typedef struct {
    ufixed_t value;      // Текущее значение EMA в unsigned fixed-point
    ufixed_t alpha;      // Коэффициент сглаживания (0 < alpha < 1)
    uint8_t initialized;
} EMA_Filter_UFixed;

void ema_ufixed_init(EMA_Filter_UFixed *filter, float alpha) {
    // Убеждаемся, что alpha в диапазоне [0,1]
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    
    filter->alpha = FLOAT_TO_UFIXED(alpha);
    filter->value = 0;
    filter->initialized = 0;
}

ufixed_t ema_ufixed_update(EMA_Filter_UFixed *filter, ufixed_t measured_value) {
    if (!filter->initialized) {
        filter->value = measured_value;
        filter->initialized = 1;
    } else {
        // EMA формула для unsigned fixed-point
        // term1 = alpha * measured
        // term2 = (1-alpha) * previous_value
        
        // Для term2 нужно вычислить (1-alpha):
        ufixed_t one_minus_alpha = (1UL << FIXED_SHIFT) - filter->alpha;
        
        // Умножение с округлением для лучшей точности
        ufixed_t term1 = ((filter->alpha * measured_value) + (1UL << (FIXED_SHIFT - 1))) >> FIXED_SHIFT;
        ufixed_t term2 = ((one_minus_alpha * filter->value) + (1UL << (FIXED_SHIFT - 1))) >> FIXED_SHIFT;
        
        filter->value = term1 + term2;
    }
    return filter->value;
}

// Оптимизированная версия для Cortex-M4 с использованием DSP инструкций
__attribute__((always_inline))
static inline ufixed_t ema_ufixed_update_cmsis(EMA_Filter_UFixed *filter, ufixed_t measured_value) {
    if (!filter->initialized) {
        filter->value = measured_value;
        filter->initialized = 1;
    } else {
      
        
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
```

## Важные моменты при использовании uint32_t

1. **Диапазон значений**:
   - Максимум: ~65535.999 (если использовать 16 бит на дробную часть)
   - Можно регулировать точность: Q20.12, Q24.8 и т.д.

2. **Округление**:
   - Добавлено `(1UL << (FIXED_SHIFT - 1))` для правильного округления при делении сдвигом

3. **Контроль переполнения**:

   ```c
   // Проверка перед умножением
   if (filter->alpha > (UINT32_MAX / measured_value)) {
       // Обработка переполнения
   }
   ```

4. **Разные форматы Q-формата**:

   ```c
   #define Q_FORMAT 16  // Можно менять
   // Q12.20 для большей точности дробной части
   // Q24.8 для большего целого диапазона
   ```

## Пример использования

```c
EMA_Filter_UFixed filter;

// Инициализация с alpha = 0.1 (медленная фильтрация)
ema_ufixed_init(&filter, 0.1f);

// Обработка данных (все значения положительные)
uint32_t raw_data[] = {100, 105, 98, 110, 95};
uint32_t filtered;

for(int i = 0; i < 5; i++) {
    filtered = ema_ufixed_update_optimized(&filter, raw_data[i]);
    // filtered теперь отфильтрованное значение в UQ16.16 формате
}
```

Использование `uint32_t` действительно оптимальнее для вашего случая, так как даёт на один бит больше для представления числа и упрощает код.
