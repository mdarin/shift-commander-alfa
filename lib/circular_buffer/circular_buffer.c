#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "circular_buffer.h"

// Для барьеров памяти на Cortex-M
#define __ARM_ARCH_7M__ 1 // TODo:
#ifdef __ARM_ARCH_7M__
#include "stm32f4xx.h" // или ваш заголовочный файл для STM32
#define MEMORY_BARRIER() __DSB()
#else
#define MEMORY_BARRIER() // empty for other platforms
#endif

// The definition of our circular buffer structure is hidden from the user
struct circular_buf_t
{
	circular_buf_data_t *buffer;
	size_t head;
	size_t tail;
	size_t max; // of the buffer
	bool full;
};

// Структура lock-free буфера

// #pragma mark - Private Functions -

static inline size_t advance_headtail_value(size_t value, size_t max)
{
	return (value + 1) % max;
}

static void advance_head_pointer(cbuf_handle_t me)
{
	assert(me);

	if (circular_buf_full(me))
	{
		me->tail = advance_headtail_value(me->tail, me->max);
	}

	me->head = advance_headtail_value(me->head, me->max);
	me->full = (me->head == me->tail);
}

// #pragma mark - APIs -

cbuf_handle_t circular_buf_init(circular_buf_data_t *buffer, size_t size)
{
	assert(buffer && size);

	cbuf_handle_t cbuf = malloc(sizeof(circular_buf_t));
	assert(cbuf);

	cbuf->buffer = buffer;
	cbuf->max = size;
	circular_buf_reset(cbuf);

	assert(circular_buf_empty(cbuf));

	return cbuf;
}

void circular_buf_free(cbuf_handle_t me)
{
	assert(me);
	free(me);
}

void circular_buf_reset(cbuf_handle_t me)
{
	assert(me);

	me->head = 0;
	me->tail = 0;
	me->full = false;
}

size_t circular_buf_size(cbuf_handle_t me)
{
	assert(me);

	size_t size = me->max;

	if (!circular_buf_full(me))
	{
		if (me->head >= me->tail)
		{
			size = (me->head - me->tail);
		}
		else
		{
			size = (me->max + me->head - me->tail);
		}
	}

	return size;
}

size_t circular_buf_capacity(cbuf_handle_t me)
{
	assert(me);

	return me->max;
}

void circular_buf_put(cbuf_handle_t me, circular_buf_data_t data)
{
	assert(me && me->buffer);

	me->buffer[me->head] = data;

	advance_head_pointer(me);
}

int circular_buf_try_put(cbuf_handle_t me, circular_buf_data_t data)
{
	int r = -1;

	assert(me && me->buffer);

	if (!circular_buf_full(me))
	{
		me->buffer[me->head] = data;
		advance_head_pointer(me);
		r = 0;
	}

	return r;
}

int circular_buf_get(cbuf_handle_t me, circular_buf_data_t *data)
{
	assert(me && data && me->buffer);

	int r = -1;

	if (!circular_buf_empty(me))
	{
		*data = me->buffer[me->tail];
		me->tail = advance_headtail_value(me->tail, me->max);
		me->full = false;
		r = 0;
	}

	return r;
}

bool circular_buf_empty(cbuf_handle_t me)
{
	assert(me);

	return (!circular_buf_full(me) && (me->head == me->tail));
}

bool circular_buf_full(cbuf_handle_t me)
{
	assert(me);

	return me->full;
}

int circular_buf_peek(cbuf_handle_t me, circular_buf_data_t *data, unsigned int look_ahead_counter)
{
	int r = -1;
	size_t pos;

	assert(me && data && me->buffer);

	// We can't look beyond the current buffer size
	if (circular_buf_empty(me) || look_ahead_counter > circular_buf_size(me))
	{
		return r;
	}

	pos = me->tail;
	for (unsigned int i = 0; i < look_ahead_counter; i++)
	{
		data[i] = me->buffer[pos];
		pos = advance_headtail_value(pos, me->max);
	}

	return 0;
}

int circular_buf_get_range(cbuf_handle_t me, circular_buf_data_t *data, size_t len)
{
	assert(me && data && me->buffer);

	// Проверяем, достаточно ли данных в буфере
	if (circular_buf_empty(me) || len > circular_buf_size(me))
	{
		return -1;
	}

	// Читаем данные по частям, учитывая возможное зацикливание
	size_t cnt = 0;
	while (cnt < len)
	{
		data[cnt] = me->buffer[me->tail];
		me->tail = advance_headtail_value(me->tail, me->max);
		cnt++;
	}

	me->full = false;

	return 0;
}

int circular_buf_put_range(cbuf_handle_t me, circular_buf_data_t *data, size_t len)
{
	assert(me && data && me->buffer);

	// Проверяем, достаточно ли свободного места
	if (len > circular_buf_capacity(me) - circular_buf_size(me))
	{
		return -1;
	}

	// Записываем данные по частям, учитывая возможное зацикливание
	for (size_t i = 0; i < len; i++)
	{
		me->buffer[me->head] = data[i];
		advance_head_pointer(me);
	}

	return 0;
}

// Альтернативная оптимизированная версия circular_buf_put_range, которая минимизирует количество вызовов advance_head_pointer
int circular_buf_put_range_fast(cbuf_handle_t me, circular_buf_data_t *data, size_t len)
{
	assert(me && data && me->buffer);

	// Проверяем, достаточно ли свободного места
	if (len > circular_buf_capacity(me) - circular_buf_size(me))
	{
		return -1;
	}

	size_t i = 0;

	// Записываем данные до конца буфера
	size_t space_to_end = me->max - me->head;
	if (space_to_end <= len)
	{
		// Копируем первую часть до конца буфера
		for (; i < space_to_end; i++)
		{
			me->buffer[me->head + i] = data[i];
		}

		// Обновляем head и tail несколько раз за одну операцию
		for (size_t j = 0; j < space_to_end; j++)
		{
			me->head = advance_headtail_value(me->head, me->max);
			if (me->head == me->tail)
			{
				me->tail = advance_headtail_value(me->tail, me->max);
			}
		}

		// Если данные не закончились, начинаем с начала буфера
		if (i < len)
		{
			size_t remaining = len - i;
			for (size_t j = 0; j < remaining; j++)
			{
				me->buffer[j] = data[i + j];
			}

			// Финальное обновление указателей
			for (size_t j = 0; j < remaining; j++)
			{
				me->head = advance_headtail_value(me->head, me->max);
				if (me->head == me->tail)
				{
					me->tail = advance_headtail_value(me->tail, me->max);
				}
			}
		}
	}
	else
	{
		// Данные помещаются без зацикливания
		for (; i < len; i++)
		{
			me->buffer[me->head + i] = data[i];
		}

		// Обновляем указатели
		for (size_t j = 0; j < len; j++)
		{
			me->head = advance_headtail_value(me->head, me->max);
			if (me->head == me->tail)
			{
				me->tail = advance_headtail_value(me->tail, me->max);
			}
		}
	}

	// Обновляем флаг full
	me->full = (me->head == me->tail);

	return 0;
}

// Используем float вместо double
float circular_buf_mean_arithmetic_f32(cbuf_handle_t me)
{
	assert(me && me->buffer);

	size_t size = circular_buf_size(me);
	if (size == 0)
		return 0.0f;

	float sum = 0.0f;
	size_t pos = me->tail;

	for (size_t i = 0; i < size; i++)
	{
		sum += (float)me->buffer[pos];
		pos = advance_headtail_value(pos, me->max);
	}

	return sum / (float)size;
}

// Использование CMSIS-DSP для RMS
float circular_buf_rms_cmsis(cbuf_handle_t me, size_t n)
{
	size_t size = circular_buf_size(me);
	if (size == 0)
		return 0.0f;

	size_t count = (n < size) ? n : size;
	float32_t temp[count]; // VLA, но лучше статический массив

	size_t pos = me->head;
	for (size_t i = 0; i < count; i++)
	{
		pos = (pos == 0) ? (me->max - 1) : (pos - 1);
		temp[count - 1 - i] = (float32_t)me->buffer[pos];
	}

	float32_t result;
	arm_rms_f32(temp, count, &result); // Аппаратно-ускоренный RMS
	return result;
}

// Медианный фильтр без malloc
#define MEDIAN_SIZE 9 // Нечетное число для медианы
float circular_buf_median_fixed(cbuf_handle_t me)
{
	static float32_t temp[MEDIAN_SIZE];
	static float32_t sorted[MEDIAN_SIZE];

	size_t pos = me->head;
	for (size_t i = 0; i < MEDIAN_SIZE; i++)
	{
		pos = (pos == 0) ? (me->max - 1) : (pos - 1);
		temp[i] = (float32_t)me->buffer[pos];
	}

	// Копируем для сортировки
	memcpy(sorted, temp, sizeof(temp));

	// Быстрая сортировка для малых массивов
	// TODO: void arm_sort_f32(const arm_sort_instance_f32 *S, float32_t *pSrc, float32_t *pDst, uint32_t blockSize)
	// arm_sort_f32(sorted, MEDIAN_SIZE, ARM_SORT_ASCENDING);

	return sorted[MEDIAN_SIZE / 2];
}

/**
 * Комбинированный фильтр: взвешенное среднее
 * Использование arm_weighted_sum_f32
 * Более новые значения имеют больший вес
 */
float circular_buf_mean_weighted_cmsis(cbuf_handle_t me)
{
	assert(me && me->buffer);

	size_t size = circular_buf_size(me);
	if (size == 0)
	{
		return 0.0f;
	}

	// Временные буферы для CMSIS
	float32_t values[size];
	float32_t weights[size];

	// Копируем значения из циклического буфера в линейный массив
	size_t pos = me->tail;
	for (size_t i = 0; i < size; i++)
	{
		values[i] = (float32_t)me->buffer[pos];

		// Вес линейно растет к новым значениям
		weights[i] = (float32_t)(i + 1) / (float32_t)size;

		pos = advance_headtail_value(pos, me->max);
	}

	float32_t result;
	// Вычисляем взвешенную сумму
	arm_dot_prod_f32(values, weights, size, &result);

	// Сумма весов = (size + 1) * size / (2 * size) = (size + 1) / 2
	float32_t weight_sum = (float32_t)(size + 1) / 2.0f;

	return result / weight_sum;
}

// Оптимизированный ассемблерный подход через intrinsics
float circular_buf_mean_weighted_intrinsic(cbuf_handle_t me)
{
	assert(me && me->buffer);

	size_t size = circular_buf_size(me);
	if (size == 0)
	{
		return 0.0f;
	}

	float32_t sum = 0.0f;
	float32_t weight_sum = 0.0f;

// Определяем, можно ли использовать SIMD
#if defined(__ARM_ARCH_7EM__) || defined(ARM_MATH_CM4)
	// Для Cortex-M4 с FPU используем CMSIS
	float32_t values[size];
	float32_t weights[size];

	size_t pos = me->tail;
	for (size_t i = 0; i < size; i++)
	{
		values[i] = (float32_t)me->buffer[pos];
		weights[i] = (float32_t)(i + 1) / (float32_t)size;
		pos = advance_headtail_value(pos, me->max);
	}

	// Используем CMSIS для быстрого скалярного произведения
	arm_dot_prod_f32(values, weights, size, &sum);
	weight_sum = (float32_t)(size + 1) / 2.0f;

#else
	// Для остальных случаев - обычный цикл
	size_t pos = me->tail;
	for (size_t i = 0; i < size; i++)
	{
		float32_t weight = (float32_t)(i + 1) / (float32_t)size;
		sum += (float32_t)me->buffer[pos] * weight;
		weight_sum += weight;
		pos = advance_headtail_value(pos, me->max);
	}
#endif

	return sum / weight_sum;
}

// #pragma mark - Lock-Free Circular Buffer Implementation -

/**
 * Оптимизация: если размер буфера - степень двойки, можно использовать
 * head = (head + 1) & (max-1) вместо head = (head + 1) % max
 */
static inline size_t advance_lf_head(lf_cbuf_handle_t me, size_t value)
{
	// Проверяем, является ли max степенью двойки
	if ((me->max & (me->max - 1)) == 0)
	{
		// Степень двойки - используем AND для скорости
		return (value + 1) & (me->max - 1);
	}
	else
	{
		// Не степень двойки - используем modulo
		return (value + 1) % me->max;
	}
}

static inline size_t advance_lf_tail(lf_cbuf_handle_t me, size_t value)
{
	return advance_lf_head(me, value); // та же логика
}

lf_cbuf_handle_t lf_circular_buf_init(lf_data_t *buffer, size_t size)
{
	assert(buffer && size > 0);

	lf_cbuf_handle_t cbuf = malloc(sizeof(lf_circular_buf_t));
	assert(cbuf);

	cbuf->buffer = buffer;
	cbuf->max = size;
	cbuf->head = 0;
	cbuf->tail = 0;

	lf_circular_buf_reset_stats(cbuf);

	return cbuf;
}

void lf_circular_buf_free(lf_cbuf_handle_t me)
{
	assert(me);
	free(me);
}

void lf_circular_buf_reset(lf_cbuf_handle_t me)
{
	assert(me);

	// Для lock-free версии нужно быть осторожным с reset'ом
	// Лучше вызывать только когда гарантированно нет активных операций
	me->head = 0;
	me->tail = 0;
	MEMORY_BARRIER();
}

size_t lf_circular_buf_size(lf_cbuf_handle_t me)
{
	assert(me);

	size_t head = me->head; // читаем volatile
	size_t tail = me->tail; // читаем volatile
	size_t size;

	if (head >= tail)
	{
		size = head - tail;
	}
	else
	{
		size = me->max - tail + head;
	}

	return size;
}

size_t lf_circular_buf_capacity(lf_cbuf_handle_t me)
{
	assert(me);
	return me->max;
}

// ! === begin
// ! эти реализации скорей всего не корректны так как буфер lock-free у него нет понятия пустой
// ! это обычная переменная, но которая работает по схеме green/blue одна в работе другая заменяется
// ! потом они меняются. Цена блокировки - удвоенный расход памяти
bool lf_circular_buf_empty(lf_cbuf_handle_t me)
{
	assert(me);
	return (me->head == me->tail);
}

bool lf_circular_buf_full(lf_cbuf_handle_t me)
{
	assert(me);

	size_t next_head = advance_lf_head(me, me->head);
	return (next_head == me->tail);
}
// ! === end

void lf_circular_buf_put(lf_cbuf_handle_t me, lf_data_t data)
{
	assert(me && me->buffer);

	size_t current_head = me->head;
	size_t next_head = advance_lf_head(me, current_head);

	// Проверка на переполнение (для статистики)
	if (next_head == me->tail)
	{
		me->overrun_count++;
	}

	// Записываем данные
	me->buffer[current_head] = data;

	// Барьер памяти: гарантируем, что запись данных завершена
	// перед обновлением head
	MEMORY_BARRIER();

	// Если буфер полон, мы просто перезаписываем - tail будет сдвинут
	// автоматически при чтении или следующей записи
	me->head = next_head;

	// Если мы перегнали tail, значит мы потеряли самые старые данные
	// но это допустимо по условию задачи

	// Обновление статистики максимального размера
	size_t current_size = lf_circular_buf_size(me);
	if (current_size > me->max_size_reached)
	{
		me->max_size_reached = current_size;
	}
}

int lf_circular_buf_try_put(lf_cbuf_handle_t me, lf_data_t data)
{
	assert(me && me->buffer);

	size_t current_head = me->head;
	size_t next_head = advance_lf_head(me, current_head);

	// Проверяем, не переполним ли мы буфер
	if (next_head == me->tail)
	{
		return -1; // буфер полон
	}

	// Записываем данные
	me->buffer[current_head] = data;
	MEMORY_BARRIER();
	me->head = next_head;

	return 0;
}

int lf_circular_buf_get(lf_cbuf_handle_t me, lf_data_t *data)
{
	assert(me && data && me->buffer);

	size_t current_tail = me->tail;

	// Проверяем, не пуст ли буфер
	if (current_tail == me->head)
	{
		me->underrun_count++; // буфер пуст
	}

	// Читаем данные
	*data = me->buffer[current_tail];

	// Барьер памяти перед обновлением tail
	MEMORY_BARRIER();

	// Обновляем tail
	me->tail = advance_lf_tail(me, current_tail);

	return 0;
}

int lf_circular_buf_peek(lf_cbuf_handle_t me, lf_data_t *data, unsigned int look_ahead_counter)
{
	assert(me && data && me->buffer);

	size_t head = me->head;
	size_t tail = me->tail;
	size_t available;

	// Вычисляем доступное количество данных
	if (head >= tail)
	{
		available = head - tail;
	}
	else
	{
		available = me->max - tail + head;
	}

	if (available < look_ahead_counter)
	{
		return -1; // недостаточно данных
	}

	// Копируем данные без изменения tail
	size_t pos = tail;
	for (unsigned int i = 0; i < look_ahead_counter; i++)
	{
		data[i] = me->buffer[pos];
		pos = advance_lf_tail(me, pos);
	}

	return 0;
}

int lf_circular_buf_get_latest(lf_cbuf_handle_t me, lf_data_t *data)
{
	assert(me && data && me->buffer);

	size_t head = me->head;
	size_t tail = me->tail;

	if (head == tail)
	{
		me->underrun_count++; // буфер пуст

		// return -1;
	}

	// Получаем индекс последнего записанного элемента
	size_t latest;
	if (head == 0)
	{
		latest = me->max - 1;
	}
	else
	{
		latest = head - 1;
	}

	// Читаем последние данные
	*data = me->buffer[latest];

	// Обновляем tail до latest (теряем все старые данные)
	MEMORY_BARRIER();
	me->tail = latest;

	return 0;
}

void lf_circular_buf_put_array(lf_cbuf_handle_t me, const lf_data_t *data, size_t count)
{
	assert(me && data && me->buffer);

	for (size_t i = 0; i < count; i++)
	{
		lf_circular_buf_put(me, data[i]);
	}
}

size_t lf_circular_buf_get_array(lf_cbuf_handle_t me, lf_data_t *data, size_t max_count)
{
	assert(me && data && me->buffer);

	size_t read_count = 0;

	while (read_count < max_count && lf_circular_buf_get(me, &data[read_count]) == 0)
	{
		read_count++;
	}

	return read_count;
}

int lf_circular_buf_peek_latest(lf_cbuf_handle_t me, lf_data_t *data)
{
	assert(me && data && me->buffer);

	if (me->head == me->tail)
	{
		return -1; // пусто
	}

	size_t latest = (me->head == 0) ? me->max - 1 : me->head - 1;
	*data = me->buffer[latest];

	return 0;
}

void lf_circular_buf_clear(lf_cbuf_handle_t me)
{
	assert(me);

	// Просто синхронизируем head и tail
	// Это безопасно, т.к. операции put/get используют volatile
	me->tail = me->head;
	MEMORY_BARRIER();
}

void lf_circular_buf_get_stats(lf_cbuf_handle_t me, lf_buffer_stats_t *stats)
{
	assert(me && stats);

	stats->size = lf_circular_buf_size(me);
	stats->capacity = me->max;
	stats->max_size = me->max_size_reached;
	stats->overruns = me->overrun_count;
	stats->underruns = me->underrun_count;
}

void lf_circular_buf_reset_stats(lf_cbuf_handle_t me)
{
	assert(me);

	me->max_size_reached = 0;
	me->overrun_count = 0;
	me->underrun_count = 0;
}
