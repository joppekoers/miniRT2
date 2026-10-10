#include "../../lib/libft/include/libft.h"
#include "vecm.h"

t_vecm vecm(size_t elements, size_t element_size, void* (*_malloc)(size_t), void (*_free)(void*))
{
	t_vecm vecm;

	if (elements == 0)
		elements = 10;
	vecm.malloc = _malloc;
	vecm.free = _free;
	vecm.vec.table = _malloc(elements * element_size);
	vecm.vec.start_i = 0;
	vecm.vec.length = 0;
	vecm.vec.byte_size = elements * element_size;
	vecm.vec.element_size = element_size;
	return (vecm);
}

static void grow(t_vecm* vecm, size_t min_byte_size)
{
	t_vec* vec = &vecm->vec;
	size_t new_size = vec->byte_size;
	do
	{
		new_size *= 2;
	} while (new_size < min_byte_size);

	void* new = vecm->malloc(new_size);
	ft_memcpy(new, vec->table + (vec->start_i * vec->element_size), vec->length * vec->element_size);
	if (vecm->free)
		vecm->free(vec->table);
	vec->table = new;
	vec->start_i = 0;
	vec->byte_size = new_size;
}

void* vecm_set(t_vecm* vecm, size_t i, void* value)
{
	const size_t min_byte_size = (i + vecm->vec.start_i + 1) * vecm->vec.element_size;
	if (min_byte_size > vecm->vec.byte_size)
		grow(vecm, min_byte_size);
	return vec_set(&vecm->vec, i, value);
}

void vecm_free(t_vecm* vecm)
{
	if (vecm->free)
		vecm->free(vecm->vec.table);
	ft_bzero(vecm, sizeof(t_vecm));
}
