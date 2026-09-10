#pragma once
#include <stdint.h>
#include <stddef.h>
#include "bare_minimum.h"

// Minimal memory arena to minimise cpy cache misses
typedef struct s_marena
{
	uint8_t* p;
	size_t	 capacity;
	size_t	 byte_index;
} t_marena;

void  marena_init(t_marena* a, size_t capacity);
void* marena_malloc(t_marena* a, size_t size);
