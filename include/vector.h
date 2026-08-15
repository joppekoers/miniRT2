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

#include "constants.h"

typedef struct s_vec
{
	size_t	 start_i;
	size_t	 length;
	size_t	 byte_size;
	size_t	 element_size;
	uint8_t* table;
} t_vec;

t_vec  vec(size_t elements, size_t element_size);
void*  vec_set(t_vec* vec, size_t i, void* value);
void*  vec_set_s(t_vec* vec, ssize_t i, void* value);
bool   vec_get(t_vec* vec, void* dest, size_t i);
bool   vec_get_s(t_vec* vec, void* dest, ssize_t i);
void*  vec_push(t_vec* vec, void* value);
void   vec_free(t_vec* vec, void (*del)(void*));
void   vec_shift(t_vec* vec, void (*del)(void*));
void   vec_pop(t_vec* vec, void (*del)(void*));

double length(t_vec3 v);
double dot(t_vec3 a, t_vec3 b);
t_vec3 cross(t_vec3 a, t_vec3 b);
t_vec3 add(t_vec3 a, t_vec3 b);
t_vec3 subtract(t_vec3 a, t_vec3 b);
t_vec3 scale(t_vec3 v, double r);
t_vec3 unit(t_vec3 v);
t_vec3 translate(t_vec3 origin, t_vec3 dir, double t);
double distance2(t_vec3 a, t_vec3 b);
double distance(t_vec3 a, t_vec3 b);
t_vec3 project_on_line(t_vec3 p, t_vec3 line0, t_vec3 line1);
t_vec3 vec3(double x, double y, double z);

typedef union u_rsqrt
{
	double	 f;
	uint64_t i;
} t_rsqrt;

void normalize(t_vec3* v);

#endif
