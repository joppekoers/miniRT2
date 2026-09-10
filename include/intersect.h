/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   intersect.h                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/05 23:43:46 by jkoers        #+#    #+#                 */
/*   Updated: 2021/02/05 23:43:46 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERSECT_H
#define INTERSECT_H

#include "constants.h"

t_hit  hit_cylinder(t_pos pos, t_ray ray);
t_hit  hit_plane(t_pos pos, t_ray ray);
t_hit  hit_sphere(t_pos pos, t_ray ray);
t_hit  hit_square(t_pos pos, t_ray ray);
t_hit  hit_triangle(t_pos pos, t_ray ray);

t_hit  hit_obj(t_shape shape, t_pos pos, t_ray ray);

void   get_edge_normal(const t_triangle* tr, t_vec3* edge1, t_vec3* edge2, t_vec3* normal);
void   set_edge_normal(t_triangle* tr);

t_vec3 correct_normal(t_vec3 normal, t_ray ray);

#endif
