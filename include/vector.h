/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   vector.h                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/01/08 15:58:29 by jkoers        #+#    #+#                 */
/*   Updated: 2021/01/18 13:59:15 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include "../lib/libft/include/libft.h"
#include <math.h>

typedef struct s_vec3
{
	double x;
	double y;
	double z;
} t_vec3;

#define dot(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define cross(a, b) \
	((t_vec3){(a).y * (b).z - (a).z * (b).y, (a).z * (b).x - (a).x * (b).z, (a).x * (b).y - (a).y * (b).x})

#define add(a, b) ((t_vec3){(a).x + (b).x, (a).y + (b).y, (a).z + (b).z})
#define subtract(a, b) ((t_vec3){(a).x - (b).x, (a).y - (b).y, (a).z - (b).z})
#define scale(v, r) ((t_vec3){(v).x * r, (v).y * r, (v).z * r})
#define vec3(x, y, z) ((t_vec3){x, y, z})
#define normalize(v) \
	({ \
		const double i = 1.0 / sqrt((v)->x * (v)->x + (v)->y * (v)->y + (v)->z * (v)->z); \
		(v)->x *= i; \
		(v)->y *= i; \
		(v)->z *= i; \
	})
#define unit(v) \
	({ \
		t_vec3 u = (v); \
		normalize(&u); \
		u; \
	})
#define length(v) (sqrt(dot(*(v), *(v))))
#define translate(origin, dir, t) (add(origin, scale(dir, t)))
#define distance2(a, b) \
	({ \
		const t_vec3 ab = subtract(*(a), *(b)); \
		ab.x* ab.x + ab.y* ab.y + ab.z* ab.z; \
	})
#define distance(a, b) (sqrt(distance2(a, b)))

t_vec3 project_on_line(t_vec3 p, t_vec3 line0, t_vec3 line1);

typedef struct s_vec
{
	size_t	 start_i;
	size_t	 length;
	size_t	 byte_size;
	size_t	 element_size;
	uint8_t* table;
	void* (*malloc)(size_t);
	void (*free)(void*);
} t_vec;

t_vec vec(size_t elements, size_t element_size);
t_vec vecm(size_t elements, size_t element_size, void* (*allocator)(size_t), void (*free)(void*));
void* vec_set(t_vec* vec, size_t i, void* value);
bool  vec_get(const t_vec* vec, void* dest, size_t i);
void  vec_free(t_vec* vec, void (*del)(void*));
void  vec_shift(t_vec* vec, void (*del)(void*));
void  vec_pop(t_vec* vec, void (*del)(void*));

// The starting byte of the element being stored at index i
#define table_byte(vec, i) vec->table + (((i) + vec->start_i) * vec->element_size)

static inline void* vec_set_s(t_vec* vec, ssize_t i, void* value)
{
	if (i < 0)
		i = vec->length + i;
	return vec_set(vec, i, value);
}

static inline void* vec_getp(const t_vec* vec, size_t i)
{
	if (i >= vec->length)
		return NULL;
	return table_byte(vec, i);
}

static inline bool vec_gets(const t_vec* vec, void* dest, ssize_t i)
{
	if (i < 0)
		i = vec->length + i;
	return vec_get(vec, dest, i);
}

static inline void* vec_push(t_vec* vec, void* value)
{
	return vec_set(vec, vec->length, value);
}

typedef union u_rsqrt
{
	double	 f;
	uint64_t i;
} t_rsqrt;

#endif
