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

t_hit hit_obj(const t_gui* gui, const t_obj* obj, t_ray ray)
{
	const void* pos = get_pos(gui, obj);
	if (obj->shape == SHAPE_TRIANGLE)
		return (hit_triangle(pos, ray));
	if (obj->shape == SHAPE_SPHERE)
		return (hit_sphere(pos, ray));
	if (obj->shape == SHAPE_PLANE)
		return (hit_plane(pos, ray));
	if (obj->shape == SHAPE_CYLINDER)
		return (hit_cylinder(pos, ray));
	exit_e("Shape not supported");
}
