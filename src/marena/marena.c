#include "marena.h"
#include <sys/mman.h>

static size_t nearest_multiple_of(size_t number, size_t multiple)
{
	size_t remainder = number % multiple;
	if (remainder == 0)
		return number;
	return number + multiple - remainder;
}

void marena_init(t_marena* a, size_t capacity)
{
	capacity = nearest_multiple_of(capacity, 4096);
	a->p = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
	debug_assert(a->p);
	a->capacity = capacity;
}

void* marena_malloc(t_marena* a, size_t size)
{
	debug_assert(size);
	debug_assert(a->byte_index + size < a->capacity); // exceeds max allocated size

	void* p = a->p + a->byte_index;
	a->byte_index += size;
	return p;
}
