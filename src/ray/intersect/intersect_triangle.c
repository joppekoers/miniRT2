/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   intersect_triangle.c                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/05 23:43:16 by jkoers        #+#    #+#                 */
/*   Updated: 2021/02/05 23:43:16 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

/*
** #include "intersect.h"
**
** #include "constants.h"
** #include "vector.h"
** #include "../lib/libft/include/libft.h"
** #include <math.h>
**
** t_hit	hit_triangle(t_pos pos, t_ray ray)
** {
** 	t_vec3 h, s, q;
** 	double a, f, u, v;
**
** 	h = cross(ray.dir, pos.tr.edge2);
** 	a = dot(pos.tr.edge1, h);
** 	if (a > -EPSILON && a < EPSILON)
** 		return ((t_hit){false}); // Ray is parallel to this triangle.
** 	f = 1.0 / a;
** 	s = subtract(ray.origin, pos.tr.p0);
** 	u = f * dot(s, h);
** 	if (u < 0.0 || u > 1.0)
** 		return ((t_hit){false});
** 	q = cross(s, pos.tr.edge1);
** 	v = f * dot(ray.dir, q);
** 	if (v < 0.0 || u + v > 1.0)
** 		return ((t_hit){false});
** 	double t = f * dot(pos.tr.edge2, q);
** 	if (t < EPSILON) // There is a line intersection but not a ray intersection
** 		return ((t_hit){false});
**
** 	t_hit	hit;
** 	hit.hit = true;
** 	hit.dist = length(scale(ray.dir, t));
** 	hit.point = scale(ray.origin, hit.dist);
** 	hit.normal = cross(subtract(pos.tr.p1, pos.tr.p0),
** 		subtract(pos.tr.p2, pos.tr.p0)); // N = (v1-v0).crossProduct(v2-v0);
** 	hit.normal = unit(scale(hit.normal, -1.0));
** 	return (hit);
** }
*/

#include "intersect.h"
#include "norm.h"
#include "constants.h"
#include "vector.h"

void set_edge_normal(t_triangle* tr)
{
	(void)tr;
#ifdef PRE_COMPUTE_TRIANGLE
	tr->edge1 = subtract(tr->p1, tr->p0);
	tr->edge2 = subtract(tr->p2, tr->p0);
	tr->normal = unit(cross(tr->edge1, tr->edge2));
#endif
}

// Stolen from: Möller–Trumbore

t_hit hit_triangle(t_pos pos, t_ray ray)
{
#ifdef PRE_COMPUTE_TRIANGLE
	const t_vec3 edge1 = pos.tr.edge1;
	const t_vec3 edge2 = pos.tr.edge2;
#else
	const t_vec3 edge1 = subtract(pos.tr.p1, pos.tr.p0);
	const t_vec3 edge2 = subtract(pos.tr.p2, pos.tr.p0);
#endif

	t_vec3 h = cross(ray.dir, edge2);
	double a = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return ((t_hit){false});

	double f = 1.0 / a;
	t_vec3 s = subtract(ray.origin, pos.tr.p0);
	double u = f * dot(s, h);
	if (u < 0.0 || u > 1.0)
		return ((t_hit){false});

	t_vec3 q = cross(s, edge1);
	double v = f * dot(ray.dir, q);
	if (v < 0.0 || u + v > 1.0)
		return ((t_hit){false});

	double t = f * dot(edge2, q);
	if (t < EPSILON)
		return ((t_hit){false});

	t_hit hit;
	hit.hit = true;
	hit.dist = t;
	hit.point = translate(ray.origin, ray.dir, hit.dist);
#ifdef PRE_COMPUTE_TRIANGLE
	const t_vec3 normal = pos.tr.normal;
#else
	const t_vec3 normal = unit(cross(edge1, edge2));
#endif
	hit.normal = correct_normal(normal, ray);
	return (hit);
}

t_aabb triangle_aabb(const t_triangle* tr)
{
	const t_vec3 min = {
			.x = min3(tr->p0.x, tr->p1.x, tr->p2.x),
			.y = min3(tr->p0.y, tr->p1.y, tr->p2.y),
			.z = min3(tr->p0.z, tr->p1.z, tr->p2.z),
	};
	const t_vec3 max = {
			.x = max3(tr->p0.x, tr->p1.x, tr->p2.x),
			.y = max3(tr->p0.y, tr->p1.y, tr->p2.y),
			.z = max3(tr->p0.z, tr->p1.z, tr->p2.z),
	};
	const t_aabb aabb = {
			.min = min,
			.max = max,
	};
	return aabb;
}

bool triangle_is_inside_aabb(const t_triangle* tr, const t_aabb* aabb)
{
	return aabb_is_inside(aabb, &tr->p0) && //
		   aabb_is_inside(aabb, &tr->p1) && //
		   aabb_is_inside(aabb, &tr->p2);
}
