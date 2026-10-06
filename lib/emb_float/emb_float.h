#ifndef EMB_FLOAT_H
#define EMB_FLOAT_H

#include <stdint.h>

/*
Использование:
#include "emb_float.h"

int id = 1001;
float temper = 23.45678f;
float hum = 67.89123f;

sprintf(buf, "Temp: " _F3 ", Hum: " _F2 ", ID: %d",
        FLOAT3(temper), FLOAT2(hum), id);
*/

// Основные макросы
#define FLOAT2INT(f, mult) \
    (uint32_t)(f), (uint32_t)(((f) - (uint32_t)(f)) * (mult) + 0.5f)

// Стандартные точночти
#define FLOAT1(f) FLOAT2INT(f, 10)
#define FLOAT2(f) FLOAT2INT(f, 100)
#define FLOAT3(f) FLOAT2INT(f, 1000)
#define FLOAT4(f) FLOAT2INT(f, 10000)
#define FLOAT5(f) FLOAT2INT(f, 100000)
#define FLOAT6(f) FLOAT2INT(f, 1000000)

// Форматы
#define _F1 "%lu.%01lu"
#define _F2 "%lu.%02lu"
#define _F3 "%lu.%03lu"
#define _F4 "%lu.%04lu"
#define _F5 "%lu.%05lu"
#define _F6 "%lu.%06lu"

// Для удобства можно добавить предустановленную точность
#ifndef FLOAT_PRECISION
#define FLOAT_PRECISION 3
#endif

#define FLOAT(f) FLOAT##FLOAT_PRECISION(f)
#define _F _F##FLOAT_PRECISION

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* EMB_FLOAT_H */
