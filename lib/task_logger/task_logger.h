#pragma once

// TODO: under construction

#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>

// Уровни логирования
#define TASK_LOG_NON 0
#define TASK_LOG_ERR 1
#define TASK_LOG_WRN 2
#define TASK_LOG_INF 3
#define TASK_LOG_DBG 4
#define TASK_LOG_VRB 5

// Флаги формата
#define TASK_LOG_FMT_TIMESTAMP (1 << 0)
#define TASK_LOG_FMT_TASKNAME (1 << 1)
#define TASK_LOG_FMT_LEVEL (1 << 2)
#define TASK_LOG_FMT_DEFAULT (TASK_LOG_FMT_TIMESTAMP | TASK_LOG_FMT_TASKNAME | TASK_LOG_FMT_LEVEL)

// Инициализация
void task_log_init(HardwareSerial *serial, int globalLogLevel = TASK_LOG_DBG, uint32_t formatFlags = TASK_LOG_FMT_DEFAULT);

// Фильтр по задачам
// Основная реализация (скрыта)
void task_log_set_filter_impl(const char *const *allowedTasks, size_t count);

// Удобный шаблон для статических массивов
template <size_t N>
inline void task_log_set_filter(const char *(&tasks)[N])
{
    task_log_set_filter_impl(tasks, N);
}

// Также оставим перегрузку с count — на случай динамического списка
inline void task_log_set_filter(const char *const *allowedTasks, size_t count)
{
    task_log_set_filter_impl(allowedTasks, count);
}

// Буферизация: true — накапливать в RAM, false — сразу выводить
void task_log_set_buffered(bool enabled);

// Принудительный сброс буфера (если включена буферизация)
void task_log_flush();

// todo проблема в получии имени задачи, функция похоже не работает нормлаьно надо изучасть вопрос
// Макросы
#define TASK_LOG_ERROR(fmt, ...) task_log_message(pcTaskGetName(NULL), TASK_LOG_ERR, fmt, ##__VA_ARGS__)
#define TASK_LOG_WARN(fmt, ...) task_log_message(pcTaskGetName(NULL), TASK_LOG_WRN, fmt, ##__VA_ARGS__)
#define TASK_LOG_INFO(fmt, ...) task_log_message(pcTaskGetName(NULL), TASK_LOG_INF, fmt, ##__VA_ARGS__)
#define TASK_LOG_DEBUG(fmt, ...) task_log_message(pcTaskGetName(NULL), TASK_LOG_DBG, fmt, ##__VA_ARGS__)
#define TASK_LOG_VERBOSE(fmt, ...) task_log_message(pcTaskGetName(NULL), TASK_LOG_VRB, fmt, ##__VA_ARGS__)
// Короткие псевдонимы для быстрого логирования
#define E(fmt, ...) TASK_LOG_ERROR(fmt, ##__VA_ARGS__)
#define W(fmt, ...) TASK_LOG_WARN(fmt, ##__VA_ARGS__)
#define I(fmt, ...) TASK_LOG_INFO(fmt, ##__VA_ARGS__)
#define D(fmt, ...) TASK_LOG_DEBUG(fmt, ##__VA_ARGS__)
#define V(fmt, ...) TASK_LOG_VERBOSE(fmt, ##__VA_ARGS__)

void task_log_message(const char *taskName, int level, const char *fmt, ...);
