#include "../../lib/libft/include/libft.h"
#include "vector.h"
#include <stdlib.h>
#include <stddef.h>

t_vec vec(size_t elements, size_t element_size)
{
	t_vec vec;

	if (elements == 0)
		elements = 10;
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

	vec->table = realloc(vec->table, new_size);
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

bool vec_get(const t_vec* vec, void* dest, size_t i)
{
	if (i >= vec->length)
		return false;
	ft_memcpy(dest, table_byte(vec, i), vec->element_size);
	return true;
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

void vec_shrink_to_fit(t_vec* vec)
{
	const size_t byte_size = (vec->length ? vec->length : 1) * vec->element_size;
	if (byte_size == vec->byte_size)
		return;

	// realloc keeps the start of the table, so the elements have to be there
	ft_memmove(vec->table, vec->table + (vec->start_i * vec->element_size), vec->length * vec->element_size);
	vec->start_i = 0;
	void* new = realloc(vec->table, byte_size);
	if (new == NULL)
		return;
	vec->table = new;
	vec->byte_size = byte_size;
}
