/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ray_to_color.c                                     :+:    :+:            */
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

// point and normal are filled in once the closest obj is known
void bounce_assign(t_bounce* b, double dist, const t_obj* obj)
{
	b->distance = dist;
	b->obj = obj;
	b->color = obj->color;
}

// Get closest t_obj * (relative to ray.origin)

t_bounce get_bounce(const t_gui* gui, t_ray ray)
{
	return octree_bounce_closest(gui, &ray);
}

// Add light from *light to *l

static void merge_lights(t_rgb* l, const t_light* light, t_bounce bounce, const t_gui* gui)
{
	const double intensity = relative_intensity(bounce.point, bounce.normal, light);

	// A surface facing away from the light gets nothing from it, so its shadow ray can be skipped
	if (intensity <= 0 || !is_clear_path(bounce, light, gui))
		return;
	*l = add_color(*l, light->color, intensity);
}
t_rgb debug_overlay(t_ray ray, const t_gui* gui, t_rgb rgb)
{
#ifdef OCTREE_DEBUG
	const t_vec3 inv_dir = {1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z};
	if (aabb_intersects(&gui->octree.aabb, &ray.origin, &inv_dir) >= 0)
		return add_color(rgb, (t_rgb){.r = 255, .g = 0, .b = 0}, 0.4);
#endif
	(void)ray;
	(void)gui;
	return rgb;
}

t_rgb ray_to_color(t_ray ray, const t_gui* gui)
{
	size_t	 i;
	t_rgb	 l;
	t_bounce bounce;

	bounce = get_bounce(gui, ray);

	if (bounce.obj == NULL)
		return debug_overlay(ray, gui, no_bounce());
	i = 0;
	l = gui->ambient.scalar;
	while (vec_getp(&gui->lights, i) != NULL)
	{
		merge_lights(&l, vec_getp(&gui->lights, i), bounce, gui);
		i++;
	}
	return debug_overlay(ray, gui, mix_color(l, bounce.color));
}
