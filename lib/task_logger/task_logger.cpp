#include <task_logger.h>
#include <cstdarg>
#include <cstring>
#include <HardwareSerial.h>

// todo скорей всего надо в класс убрать это всё
// TODO: under construction

// Настройки буфера
#ifndef TASK_LOG_BUFFER_SIZE
#define TASK_LOG_BUFFER_SIZE 512
#endif

static HardwareSerial *gSerial = nullptr;
static int gGlobalLogLevel = TASK_LOG_DBG;
static uint32_t gFormatFlags = TASK_LOG_FMT_DEFAULT;
static SemaphoreHandle_t gLogMutex = nullptr;

// Фильтр задач
static const char *const *gAllowedTasks = nullptr;
static size_t gAllowedTaskCount = 0;

// Буферизация
static bool gBuffered = false;
static char gLogBuffer[TASK_LOG_BUFFER_SIZE];
static size_t gBufferPos = 0;

void task_log_set_filter_impl(const char *const *allowedTasks, size_t count)
{
    gAllowedTasks = allowedTasks;
    gAllowedTaskCount = count;
}

// Проверка разрешения задачи
static bool is_task_allowed(const char *taskName)
{
    if (gAllowedTasks == nullptr || gAllowedTaskCount == 0)
        return true;
    if (!taskName)
        return false;
    for (size_t i = 0; i < gAllowedTaskCount; ++i)
    {
        if (gAllowedTasks[i] && strcmp(taskName, gAllowedTasks[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// Преобразование уровня
static const char *level_to_str(int level)
{
    switch (level)
    {
    case TASK_LOG_ERR:
        return "ERR";
    case TASK_LOG_WRN:
        return "WRN";
    case TASK_LOG_INF:
        return "INF";
    case TASK_LOG_DBG:
        return "DBG";
    case TASK_LOG_VRB:
        return "VRB";
    default:
        return "???";
    }
}

// Вывод строки — либо сразу, либо в буфер
static void log_output(const char *str)
{
    size_t len = strlen(str);
    if (gBuffered)
    {
        if (gBufferPos + len < sizeof(gLogBuffer))
        {
            memcpy(gLogBuffer + gBufferPos, str, len);
            gBufferPos += len;
        } // иначе обрезаем // todo (можно добавить переполнение → flush)
    }
    else
    {
        if (gSerial)
            gSerial->print(str);
    }
}

// Сброс буфера
void task_log_flush()
{
    if (!gBuffered || gBufferPos == 0 || !gSerial)
        return;
    if (xSemaphoreTake(gLogMutex, pdMS_TO_TICKS(10)) != pdTRUE)
        return;
    gSerial->write((uint8_t *)gLogBuffer, gBufferPos);
    gBufferPos = 0;
    xSemaphoreGive(gLogMutex);
}

// Инициализация
void task_log_init(HardwareSerial *serial, int globalLogLevel, uint32_t formatFlags)
{
    if (serial == nullptr)
        return;

    if (!serial->availableForWrite())
        return;

    gSerial = serial;
    gGlobalLogLevel = globalLogLevel;
    gFormatFlags = formatFlags;
    if (gLogMutex == nullptr)
    {
        gLogMutex = xSemaphoreCreateMutex();
    }
}

void task_log_set_buffered(bool enabled)
{
    if (enabled == gBuffered)
        return;
    if (!enabled)
    {
        task_log_flush(); // сбросить старый буфер
    }
    gBuffered = enabled;
}

void task_log_message(const char *taskName, int level, const char *fmt, ...)
{
    if (!gSerial || level > gGlobalLogLevel)
        return;
    if (!is_task_allowed(taskName))
        return;

    if (xSemaphoreTake(gLogMutex, pdMS_TO_TICKS(10)) != pdTRUE)
    {
        return;
    }

    char header[64] = {0};
    char *p = header;

    // todo FIXME: тут что не рабоатет и приводит к зависанию контроллера
    // if (gFormatFlags & TASK_LOG_FMT_TIMESTAMP)
    // {
    //     TickType_t ticks = xTaskGetTickCount();
    //     p += snprintf(p, sizeof(header) - (p - header), "[%lu] ", (unsigned long)ticks);
    // }

    if ((gFormatFlags & TASK_LOG_FMT_TASKNAME) && taskName)
    {
        p += snprintf(p, sizeof(header) - (p - header), "[ %s ]: ", taskName);
    }

    if (gFormatFlags & TASK_LOG_FMT_LEVEL)
    {
        p += snprintf(p, sizeof(header) - (p - header), "%s ", level_to_str(level));
    }

    log_output(header);

    // Форматирование основного сообщения
    va_list args;
    va_start(args, fmt);
    //! ограничение на длину сообщения
    char msg[128] = {0};
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    log_output(msg);
    log_output("\r\n");

    if (!gBuffered)
    {
        // Если не буферизуем — принудительно отправляем (особенно важно для CDC)
        gSerial->flush();
    }

    xSemaphoreGive(gLogMutex);
}
