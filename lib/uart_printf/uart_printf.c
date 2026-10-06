#include <uart_printf.h>

// todo
// define configUART_PRINTF_MAXLINE
// defien configUART_PRINTF_TIMEOUT

// Для печати в UART или другой периферии
void uart_printf(UART_HandleTypeDef *huart, const char *format, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Отправка в UART
    HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), 1000);
}

// * EXAMPLE
// Использование с UART и emb_float
// void uart_example(void)
// {
//     float temp = 23.456f;
//     int id = 5;
//
//     uart_printf(&huart2, "ID: %d, Temp: " _F3 "\r\n", id, F3(temp));
// }
