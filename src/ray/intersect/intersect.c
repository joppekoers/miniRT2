/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   intersect.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/05 23:47:21 by jkoers        #+#    #+#                 */
/*   Updated: 2021/02/05 23:47:21 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "intersect.h"
#include "constants.h"
#include "vector.h"

// Get the normal that points most towards ray.origin

t_vec3 correct_normal(t_vec3 normal, t_ray ray)
{
	if (dot(normal, ray.dir) > EPSILON)
		normal = scale(normal, -1);
	return (normal);
}

bool hit_obj(const t_gui* gui, const t_obj* obj, t_ray ray, double* dist)
{
	const void* pos = get_pos(gui, obj);
	if (obj->shape == SHAPE_TRIANGLE)
		return (hit_triangle(pos, ray, dist));
	if (obj->shape == SHAPE_SPHERE)
		return (hit_sphere(pos, ray, dist));
	if (obj->shape == SHAPE_PLANE)
		return (hit_plane(pos, ray, dist));
	if (obj->shape == SHAPE_CYLINDER)
		return (intersect_cylinder(ray, *(const t_cylinder*)pos, dist));
	exit_e("Shape not supported");
}

t_vec3 normal_obj(const t_gui* gui, const t_obj* obj, t_vec3 point, t_ray ray)
{
	const void* pos = get_pos(gui, obj);
	if (obj->shape == SHAPE_TRIANGLE)
		return (normal_triangle(pos, ray));
	if (obj->shape == SHAPE_SPHERE)
		return (normal_sphere(pos, point, ray));
	if (obj->shape == SHAPE_PLANE)
		return (correct_normal(((const t_plane*)pos)->normal, ray));
	if (obj->shape == SHAPE_CYLINDER)
		return (cylinder_normal(point, ray, *(const t_cylinder*)pos));
	exit_e("Shape not supported");
}
