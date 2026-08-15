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
#include "float.h"

void bounce_assign(t_bounce* b, const t_hit* hit, const t_obj* obj)
{
	b->distance = hit->dist;
	b->obj = obj;
	b->color = obj->color;
	b->point = hit->point;
	b->normal = hit->normal;
}

void bounce_nobounce(t_bounce* b)
{
	b->obj = NULL;
	b->distance = DBL_MAX;
}

// Get closest t_obj * (relative to ray.origin) from *shapes

t_bounce get_bounce(const t_gui* gui, t_ray ray)
{
#ifdef USE_OCTREE
	return octree_bounce(&gui->octree, &ray);
#else

	size_t	 i;
	t_obj*	 obj;
	t_hit	 hit;
	t_bounce bounce;

	i = 0;
	bounce_nobounce(&bounce);
	while (ft_arr_get(gui->shapes, i) != NULL)
	{
		obj = ft_arr_get(gui->shapes, i);
		hit = hit_obj(obj->shape, obj->pos, ray);
		if (hit.hit && hit.dist < bounce.distance)
			bounce_assign(&bounce, &hit, obj);
		i++;
	}
	return (bounce);
#endif
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
	while (ft_arr_get(gui->lights, i) != NULL)
	{
		merge_lights(&l, ft_arr_get(gui->lights, i), bounce, gui);
		i++;
	}
	return debug_overlay(ray, gui, mix_color(l, bounce.color));
}
