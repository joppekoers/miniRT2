#ifndef VECM_H
#define VECM_H

#include "vector.h"

typedef struct s_vecm
{
	t_vec vec;
	void* (*malloc)(size_t);
	// NULL for allocators that cannot free
	void (*free)(void*);
} t_vecm;

t_vecm				vecm(size_t elements, size_t element_size, void* (*_malloc)(size_t), void (*_free)(void*));
void*				vecm_set(t_vecm* vecm, size_t i, void* value);
void				vecm_free(t_vecm* vecm);

static inline void* vecm_push(t_vecm* vecm, void* value)
{
	return vecm_set(vecm, vecm->vec.length, value);
}

#endif
