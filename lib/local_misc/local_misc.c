#include <local_misc.h>

/**
 * @brief
 * @retval None
 */
// Медианный фильтр
#define FILTER_WINDOW_SIZE 5 // для вычисления медианы необходимо нечётное колчичество элементов
uint32_t median_filter(uint32_t *buffer, uint8_t size)
{
    uint32_t temp[FILTER_WINDOW_SIZE];
    uint8_t i, j;

    // Копируем во временный буфер
    memcpy(temp, buffer, size * sizeof(uint32_t));
    for (i = 0; i < size; i++)
    {
        temp[i] = buffer[i];
    }

    // todo заменить на частичную partial sort
    // Сортировка пузырьком
    for (i = 0; i < size - 1; i++)
    {
        for (j = 0; j < size - i - 1; j++)
        {
            if (temp[j] > temp[j + 1])
            {
                uint32_t tmp = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = tmp;
            }
        }
    }

    // Возвращаем медиану
    return temp[size / 2];
}

/**
 * @brief
 * @retval None
 */
bool array_min_safe(const uint32_t *arr, uint32_t size, uint32_t *result)
{
    if (arr == NULL || size == 0 || result == NULL)
        return false;

    *result = *arr++;
    for (uint32_t i = 1; i < size; i++)
    {
        if (*arr < *result)
            *result = *arr;
        arr++;
    }
    return true;
}

/**
 * @brief
 * @retval None
 */
bool array_max_safe(const uint32_t *arr, uint32_t size, uint32_t *result)
{
    if (arr == NULL || size == 0 || result == NULL)
        return false;

    *result = *arr++;
    for (uint32_t i = 1; i < size; i++)
    {
        if (*arr > *result)
            *result = *arr;
        arr++;
    }
    return true;
}

/**
 * @brief  todo
 * @param  htim : TIM handle
 * @note // TODO: может сделать выбор или обёртку на эти функциями чтобы была одна интерфейсная функция удобная
 * @retval None
 */
// Установка скважности PWM (0-100%)
void set_PWM_duty16(TIM_HandleTypeDef *htim, uint32_t Channel, uint8_t duty_percent)
{
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(htim);
    uint16_t pulse = (uint16_t)(period * (float)duty_percent / 100.0f);

    __HAL_TIM_SET_COMPARE(htim, Channel, pulse);
}

/**
 * @brief  todo
 * @param  htim : TIM handle
 * @note // TODO: может сделать выбор или обёртку на эти функциями чтобы была одна интерфейсная функция удобная
 * @retval None
 */
void set_PWM_duty32(TIM_HandleTypeDef *htim, uint32_t Channel, uint8_t duty_percent)
{
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(htim);
    uint32_t pulse = (uint32_t)(period * (float)duty_percent / 100.0f);

    __HAL_TIM_SET_COMPARE(htim, Channel, pulse);
}
