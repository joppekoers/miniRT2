#include "../../lib/libft/include/libft.h"
#include "vector.h"
#include <stdlib.h>
#include <stddef.h>

// The starting byte of the element being stored at index i
#define table_byte(vec, i) vec->table + (((i) + vec->start_i) * vec->element_size)

t_vec vec(size_t elements, size_t element_size)
{
	t_vec vec;

	if (elements == 0)
		elements = 100;
	vec.table = malloc(elements * element_size);
	vec.start_i = 0;
	vec.length = 0;
	vec.byte_size = elements * element_size;
	vec.element_size = element_size;
	return (vec);
}

static void grow(t_vec* vec, size_t min_byte_size)
{
	size_t new_size = vec->byte_size;
	do
	{
		new_size *= 2;
	} while (new_size < min_byte_size);

	void* new = malloc(new_size);
	ft_memcpy(new, vec->table + (vec->start_i * vec->element_size), (vec->length * vec->element_size));
	free(vec->table);
	vec->table = new;
	vec->start_i = 0;
	vec->byte_size = new_size;
}

void* vec_set(t_vec* vec, size_t i, void* value)
{
	const size_t min_byte_size = (i + vec->start_i + 1) * vec->element_size;
	if (min_byte_size > vec->byte_size)
		grow(vec, min_byte_size);

	uint8_t* dst = table_byte(vec, i);
	ft_memcpy(dst, ((uint8_t*)value), vec->element_size);
	if (vec->length <= i)
		vec->length = i + 1;
	return dst;
}

void* vec_set_s(t_vec* vec, ssize_t i, void* value)
{
	if (i < 0)
		i = vec->length + i;
	return vec_set(vec, i, value);
}

bool vec_get(const t_vec* vec, void* dest, size_t i)
{
	if (i >= vec->length)
		return false;
	ft_memcpy(dest, table_byte(vec, i), vec->element_size);
	return true;
}

void* vec_getp(const t_vec* vec, size_t i)
{
	if (i >= vec->length)
		return NULL;
	return table_byte(vec, i);
}

bool vec_gets(const t_vec* vec, void* dest, ssize_t i)
{
	if (i < 0)
		i = vec->length + i;
	return vec_get(vec, dest, i);
}

void* vec_push(t_vec* vec, void* value)
{
	return vec_set(vec, vec->length, value);
}

void vec_free(t_vec* vec, void (*del)(void*))
{
	size_t i;

	if (del)
	{
		i = 0;
		while (i < vec->length)
		{
			del((void*)(table_byte(vec, i)));
			i++;
		}
	}
	free(vec->table);
	ft_bzero(vec, sizeof(t_vec));
}

void vec_shift(t_vec* vec, void (*del)(void*))
{
	if (vec->length == 0)
		return;
	if (del)
		del((void*)(table_byte(vec, 0)));
	vec->start_i += 1;
	vec->length -= 1;
}

void vec_pop(t_vec* vec, void (*del)(void*))
{
	if (vec->length == 0)
		return;
	if (del)
		del((void*)(table_byte(vec, vec->length - 1)));
	vec->length -= 1;
}
