#include <EMA.h>

void ema_ufixed_init(EMA_Filter_UFixed *filter, float alpha)
{
    // Убеждаемся, что alpha в диапазоне [0,1]
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;

    filter->alpha = FLOAT_TO_UFIXED(alpha);
    filter->value = 0;
    filter->initialized = 0;
}

ufixed_t ema_ufixed_update(EMA_Filter_UFixed *filter, ufixed_t measured_value)
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

        // Для term2 нужно вычислить (1-alpha):
        ufixed_t one_minus_alpha = (1UL << FIXED_SHIFT) - filter->alpha;

        // Умножение с округлением для лучшей точности
        ufixed_t term1 = ((filter->alpha * measured_value) + (1UL << (FIXED_SHIFT - 1))) >> FIXED_SHIFT;
        ufixed_t term2 = ((one_minus_alpha * filter->value) + (1UL << (FIXED_SHIFT - 1))) >> FIXED_SHIFT;

        filter->value = term1 + term2;
    }
    return filter->value;
}

/*

Пример использования


EMA_Filter_UFixed filter;

Инициализация с alpha = 0.1 (медленная фильтрация)
ema_ufixed_init(&filter, 0.1f);

Обработка данных (все значения положительные)
uint32_t raw_data[] = {100, 105, 98, 110, 95};

uint32_t filtered;
float32_t result;

for(int i = 0; i < 5; i++) {
    filtered = ema_ufixed_update_cmsis(&filter, raw_data[i]);

    filtered теперь отфильтрованное значение в UQ16.16 формате

    result = UFIXED_TO_FLOAT(filtered)

    result теперь отфильтрованное значение во float32_t формате
}
*/

// Самая простая реализация - все в одной функции
// measured_value - текущее измерение
// prev_ema - предыдущее значение EMA
// alpha - коэффициент (0 < alpha ≤ 1, например, 0.1 для 10 периодов)
float32_t ema_simple(float32_t measured_value, float32_t prev_ema, float32_t alpha)
{
    return (alpha * measured_value) + ((1.0f - alpha) * prev_ema);
}

/*
Использование:

float current_ema = 0;
float alpha = 0.1f; // Чем меньше alpha, тем больше сглаживание

Инициализация первым значением
current_ema = get_measured_value();

В основном цикле
while (1)
{
    float measured = get_measured_value();
    current_ema = ema(measured, current_ema, alpha);

    Используем current_ema
}
    */
