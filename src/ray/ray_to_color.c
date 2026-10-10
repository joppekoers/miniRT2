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

void bounce_assign(t_bounce* b, const t_hit* hit, const t_obj* obj)
{
	b->distance = hit->dist;
	b->obj = obj;
	b->color = obj->color;
	b->point = hit->point;
	b->normal = hit->normal;
}

// Get closest t_obj * (relative to ray.origin)

t_bounce get_bounce(const t_gui* gui, t_ray ray)
{
	return octree_bounce_closest(gui, &ray);
}

// Add light from *light to *l

static void merge_lights(t_rgb* l, const t_light* light, t_bounce bounce, const t_gui* gui)
{
	double intensity;

	if (!is_clear_path(bounce, light, gui))
		return;
	intensity = relative_intensity(bounce.point, bounce.normal, light);
	*l = add_color(*l, light->color, intensity);
}
t_rgb debug_overlay(t_ray ray, const t_gui* gui, t_rgb rgb)
{
#ifdef OCTREE_DEBUG
	if (aabb_intersects(&gui->octree.aabb, &ray) >= 0)
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
