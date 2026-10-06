#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <arm_math.h> // CMSIS-DSP библиотека от ARM

#ifdef __cplusplus
extern "C"
{
#endif

#define CIRCULAR_BUFFER_SUCCESS 0

    /// Opaque circular buffer structure
    typedef struct circular_buf_t circular_buf_t;

    /// circular buffer data element type
    typedef uint32_t circular_buf_data_t; // TODO: Ajust buffer type here

    /// Handle type, the way users interact with the API
    typedef circular_buf_t *cbuf_handle_t;

    /// Pass in a storage buffer and size, returns a circular buffer handle
    /// Requires: buffer is not NULL, size > 0 (size > 1 for the threadsafe
    //  version, because it holds size - 1 elements)
    /// Ensures: me has been created and is returned in an empty state
    cbuf_handle_t circular_buf_init(circular_buf_data_t *buffer, size_t size);

    /// Free a circular buffer structure
    /// Requires: me is valid and created by circular_buf_init
    /// Does not free data buffer; owner is responsible for that
    void circular_buf_free(cbuf_handle_t me);

    /// Reset the circular buffer to empty, head == tail. Data not cleared
    /// Requires: me is valid and created by circular_buf_init
    void circular_buf_reset(cbuf_handle_t me);

    /// Put that continues to add data if the buffer is full
    /// Old data is overwritten
    /// Note: if you are using the threadsafe version, this API cannot be used, because
    /// it modifies the tail pointer in some cases. Use circular_buf_try_put instead.
    /// Requires: me is valid and created by circular_buf_init
    void circular_buf_put(cbuf_handle_t me, circular_buf_data_t data);

    /// Put that rejects new data if the buffer is full
    /// Note: if you are using the threadsafe version, *this* is the put you should use
    /// Requires: me is valid and created by circular_buf_init
    /// Returns 0 on success, -1 if buffer is full
    int circular_buf_try_put(cbuf_handle_t me, circular_buf_data_t data);

    /// Retrieve a value from the buffer
    /// Requires: me is valid and created by circular_buf_init
    /// Returns 0 on success, -1 if the buffer is empty
    int circular_buf_get(cbuf_handle_t me, circular_buf_data_t *data);

    /// CHecks if the buffer is empty
    /// Requires: me is valid and created by circular_buf_init
    /// Returns true if the buffer is empty
    bool circular_buf_empty(cbuf_handle_t me);

    /// Checks if the buffer is full
    /// Requires: me is valid and created by circular_buf_init
    /// Returns true if the buffer is full
    bool circular_buf_full(cbuf_handle_t me);

    /// Check the capacity of the buffer
    /// Requires: me is valid and created by circular_buf_init
    /// Returns the maximum capacity of the buffer
    size_t circular_buf_capacity(cbuf_handle_t me);

    /// Check the number of elements stored in the buffer
    /// Requires: me is valid and created by circular_buf_init
    /// Returns the current number of elements in the buffer
    size_t circular_buf_size(cbuf_handle_t me);

    /// Look ahead at values stored in the circular buffer without removing the data
    /// Requires:
    ///		- me is valid and created by circular_buf_init
    ///		- look_ahead_counter is less than or equal to the value returned by circular_buf_size()
    /// Returns 0 if successful, -1 if data is not available
    int circular_buf_peek(cbuf_handle_t me, circular_buf_data_t *data, unsigned int look_ahead_counter);

    int circular_buf_get_range(cbuf_handle_t me, circular_buf_data_t *data, size_t len);
    int circular_buf_put_range(cbuf_handle_t me, circular_buf_data_t *data, size_t len);
    int circular_buf_put_range_fast(cbuf_handle_t me, circular_buf_data_t *data, size_t len);

    // #pragma mark - Фильтры на основе циклического буфера -

    // Рекомендации по применению:
    // Тип сигнала                                Рекомендуемый фильтр     Пояснение
    // -------------------------------------------------------------------------------------------------------
    // Медленно меняющиеся(температура, давление) Скользящее среднее       Хорошо убирает высокочастотный шум
    // Переменный ток, вибрации                   RMS                      Показывает реальную мощность
    // Звук, освещенность                         Среднее геометрическое   Логарифмическое восприятие
    // С выбросами(помехи)                        Медианный                Отлично убирает импульсный шум
    // Быстрые процессы                           Взвешенное среднее       Больший вес свежих данных

    /**
     * Вычисляет среднее арифметическое всех значений в буфере
     * Полезно для сглаживания шумов, фильтрации низких частот
     */
    float circular_buf_mean_arithmetic_f32(cbuf_handle_t me);
    /**
     * Вычисляет среднее квадратическое (RMS) значений в буфере
     * Идеально для анализа мощности сигнала, переменного тока, вибраций
     */
    float circular_buf_rms_cmsis(cbuf_handle_t me, size_t n);
    /**
     * Медианный фильтр - отличный для удаления импульсных помех
     * Требует сортировки, поэтому менее эффективен
     */
    float circular_buf_median_fixed(cbuf_handle_t me);
    /**
     * Комбинированный фильтр: взвешенное среднее
     * Более новые значения имеют больший вес
     */
    float circular_buf_mean_weighted_cmsis(cbuf_handle_t me);
    float circular_buf_mean_weighted_intrinsic(cbuf_handle_t me);

    // #pragma mark - Multi-producer/multi-consumer lock-free circular buffer -

    /**
     * @brief Multi-producer/multi-consumer lock-free circular buffer
     * для случаев с допустимой потерей данных
     */

    // Тип данных для lock-free буфера (можно использовать тот же circular_buf_data_t)
    typedef circular_buf_data_t lf_data_t;

    typedef struct lf_circular_buf_t
    {
        lf_data_t *buffer;
        volatile size_t head; // индекс для записи (писатель)
        volatile size_t tail; // индекс для чтения (читатель)
        size_t max;           // размер буфера

        // Статистика (опционально)
        size_t max_size_reached;
        uint32_t overrun_count;
        uint32_t underrun_count;
    } lf_circular_buf_t;

    typedef lf_circular_buf_t *lf_cbuf_handle_t;

    // #pragma mark - Lock-Free API -

    /**
     * @brief Инициализация lock-free буфера
     * @param buffer Предварительно выделенный буфер данных
     * @param size Размер буфера (должен быть степенью двойки для оптимизации)
     */
    lf_cbuf_handle_t lf_circular_buf_init(lf_data_t *buffer, size_t size);

    /**
     * @brief Освобождение ресурсов буфера
     */
    void lf_circular_buf_free(lf_cbuf_handle_t me);

    /**
     * @brief Сброс буфера (только когда нет активных операций!)
     */
    void lf_circular_buf_reset(lf_cbuf_handle_t me);

    /**
     * @brief Получить текущий размер (количество элементов в буфере)
     */
    size_t lf_circular_buf_size(lf_cbuf_handle_t me);

    /**
     * @brief Получить вместимость буфера
     */
    size_t lf_circular_buf_capacity(lf_cbuf_handle_t me);

    /**
     * @brief Проверить, пуст ли буфер
     */
    bool lf_circular_buf_empty(lf_cbuf_handle_t me);

    /**
     * @brief Проверить, полон ли буфер
     */
    bool lf_circular_buf_full(lf_cbuf_handle_t me);

    /**
     * @brief Блокирующая запись (всегда успешна, перезаписывает старые данные)
     * @note Для single-producer использования
     */
    void lf_circular_buf_put(lf_cbuf_handle_t me, lf_data_t data);

    /**
     * @brief Неблокирующая запись (возвращает ошибку если буфер полон)
     * @return 0 если успешно, -1 если буфер полон
     */
    int lf_circular_buf_try_put(lf_cbuf_handle_t me, lf_data_t data);

    /**
     * @brief Блокирующее чтение (ждет данные)
     * @return 0 если успешно, -1 если буфер пуст
     */
    int lf_circular_buf_get(lf_cbuf_handle_t me, lf_data_t *data);

    /**
     * @brief Неблокирующее чтение с просмотром вперед
     * @param look_ahead_counter Сколько элементов просмотреть
     * @return 0 если успешно, -1 если недостаточно данных
     */
    int lf_circular_buf_peek(lf_cbuf_handle_t me, lf_data_t *data, unsigned int look_ahead_counter);

    /**
     * @brief "Умное" чтение - читает самые свежие данные, пропуская устаревшие
     * @return 0 если успешно, -1 если буфер пуст
     */
    int lf_circular_buf_get_latest(lf_cbuf_handle_t me, lf_data_t *data);

    /**
     * @brief Мульти-запись - записывает массив данных
     */
    void lf_circular_buf_put_array(lf_cbuf_handle_t me, const lf_data_t *data, size_t count);

    /**
     * @brief Мульти-чтение - читает массив данных
     * @return Количество реально прочитанных элементов
     */
    size_t lf_circular_buf_get_array(lf_cbuf_handle_t me, lf_data_t *data, size_t max_count);

    /**
     * @brief Получить статистику использования буфера
     */
    typedef struct
    {
        size_t size;        // текущий размер
        size_t capacity;    // вместимость
        size_t max_size;    // максимальный достигнутый размер
        uint32_t overruns;  // количество переполнений (потерь данных)
        uint32_t underruns; // количество попыток чтения из пустого буфера
    } lf_buffer_stats_t;

    void lf_circular_buf_get_stats(lf_cbuf_handle_t me, lf_buffer_stats_t *stats);
    void lf_circular_buf_reset_stats(lf_cbuf_handle_t me);

    /**
     * @brief Очистить буфер (безопасно)
     */
    void lf_circular_buf_clear(lf_cbuf_handle_t me);

    /**
     * @brief Прочитать данные без удаления из буфера
     */
    int lf_circular_buf_peek_latest(lf_cbuf_handle_t me, lf_data_t *data);

#ifdef __cplusplus
}
#endif

#endif // CIRCULAR_BUFFER_H_
