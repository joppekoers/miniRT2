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

int    intersect_cylinder(t_ray ray, t_cylinder cylinder, double* t);
bool   hit_plane(const t_plane* pl, t_ray ray, double* dist);
bool   hit_sphere(const t_sphere* sp, t_ray ray, double* dist);
bool   hit_triangle(const t_triangle* tr, t_ray ray, double* dist);

t_vec3 cylinder_normal(t_vec3 hitpoint, t_ray ray, t_cylinder cylinder);
t_vec3 normal_sphere(const t_sphere* sp, t_vec3 point, t_ray ray);
t_vec3 normal_triangle(const t_triangle* tr, t_ray ray);

// Only finds the distance, the point and normal are only needed for the closest hit
bool   hit_obj(const t_gui* gui, const t_obj* obj, t_ray ray, double* dist);
t_vec3 normal_obj(const t_gui* gui, const t_obj* obj, t_vec3 point, t_ray ray);

void   get_edge_normal(const t_triangle* tr, t_vec3* edge1, t_vec3* edge2, t_vec3* normal);
void   set_edge_normal(t_triangle* tr);

t_vec3 correct_normal(t_vec3 normal, t_ray ray);

#endif
