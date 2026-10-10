/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   is_clear_path.c                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/12 16:15:37 by jkoers        #+#    #+#                 */
/*   Updated: 2021/02/12 16:15:37 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ray.h"
#include "intersect.h"
#include "constants.h"
#include "vector.h"

// Pointing a new ray to hitpoint, form origin *l

t_bounce bounce_from_light(t_vec3 hitpoint, const t_light* l, const t_gui* gui)
{
	t_bounce from_light;
	t_ray	 ray;

	ray.origin = l->origin;
	ray.dir = unit(subtract(hitpoint, l->origin));
	from_light = octree_bounce_first(gui, &ray, hitpoint);
	return (from_light);
}

// If nothing blocks the path between the detected bounce and the light *l
// Assuming from_camera has bounced

bool is_clear_path(t_bounce from_camera, const t_light* l, const t_gui* gui)
{
	return (bounce_from_light(from_camera.point, l, gui).obj == NULL);
}
