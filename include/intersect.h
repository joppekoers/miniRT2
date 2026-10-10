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

t_hit  hit_cylinder(const t_cylinder* cy, t_ray ray);
t_hit  hit_plane(const t_plane* pl, t_ray ray);
t_hit  hit_sphere(const t_sphere* sp, t_ray ray);
t_hit  hit_triangle(const t_triangle* tr, t_ray ray);

t_hit  hit_obj(const t_gui* gui, const t_obj* obj, t_ray ray);

void   get_edge_normal(const t_triangle* tr, t_vec3* edge1, t_vec3* edge2, t_vec3* normal);
void   set_edge_normal(t_triangle* tr);

t_vec3 correct_normal(t_vec3 normal, t_ray ray);

#endif
