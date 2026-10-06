#ifndef UART_PRINTF_H
#define UART_PRINTF_H

#ifdef __cplusplus
extern "C"
{
#endif
    /* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <emb_float.h>
#include "usart.h"

    /* Exported constants --------------------------------------------------------*/

    /* Exported macro ------------------------------------------------------------*/

    /* Exported types ------------------------------------------------------------*/

    /* Exported constants --------------------------------------------------------*/

    /* Exported macro ------------------------------------------------------------*/

    /* Exported functions prototypes ---------------------------------------------*/
    void uart_printf(UART_HandleTypeDef *huart, const char *format, ...);
    /* Exported variables -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UART_PRINTF_H */
