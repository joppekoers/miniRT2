/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   intersect_cylinder.c                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/05 23:43:16 by jkoers        #+#    #+#                 */
/*   Updated: 2021/02/05 23:43:16 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "intersect.h"
#include "constants.h"
#include "vector.h"
#include <math.h>
#include "quadratic.h"

t_vec3 cylinder_normal(t_vec3 hitpoint, t_ray ray, t_cylinder cylinder)
{
	t_vec3 ctp;
	t_vec3 normal;

	ctp = subtract(hitpoint, cylinder.origin);
	normal = subtract(ctp, scale(cylinder.dir, dot(&cylinder.dir, &ctp)));
	normalize(&normal);
	return (correct_normal(normal, ray));
}

// WARNING!
// Starting from this point, the code is shit because it wasn't written by me

void check_t(double* t, t_cylinder cylinder, t_ray ray)
{
	t_vec3 q;
	t_vec3 p2;
	t_vec3 q_to_base;
	t_vec3 q_to_top;

	p2 = add(cylinder.origin, scale(cylinder.dir, cylinder.height));
	q = add(ray.origin, scale(ray.dir, *t));
	q_to_base = subtract(q, cylinder.origin);
	q_to_top = subtract(q, p2);
	if (dot(&cylinder.dir, &q_to_base) <= 0)
		*t = -1;
	if (dot(&cylinder.dir, &q_to_top) >= 0)
		*t = -1;
}

int cyl_get_roots(double* t0, double* t1, t_cylinder cylinder, t_ray ray)
{
	t_vec3 a_sqrt;
	t_vec3 right;
	t_vec3 oc;
	double a;
	double b;
	double c;

	a_sqrt = subtract(ray.dir, scale(cylinder.dir, dot(&ray.dir, &cylinder.dir)));
	a = dot(&a_sqrt, &a_sqrt);
	oc = subtract(ray.origin, cylinder.origin);
	right = subtract(oc, scale(cylinder.dir, dot(&oc, &cylinder.dir)));
	b = 2 * dot(&a_sqrt, &right);
	c = dot(&right, &right) - (cylinder.radius * cylinder.radius);
	if (!solve_quadratic((t_quadratic){a, b, c}, t0, t1))
		return (0);
	return (1);
}

int intersect_cylinder(t_ray ray, t_cylinder cylinder, double* t)
{
	double t0;
	double t1;

	if (!cyl_get_roots(&t0, &t1, cylinder, ray))
		return (0);
	if (t0 > 0)
		check_t(&t0, cylinder, ray);
	if (t1 > 0)
		check_t(&t1, cylinder, ray);
	if (t0 < 0 && t1 < 0)
		return (0);
	if ((t1 < t0 && t1 > 0) || (t0 < t1 && t0 <= 0))
		*t = t1;
	else
		*t = t0;
	return (1);
}

t_hit hit_cylinder(t_pos pos, t_ray ray)
{
	t_hit hit;

	hit.hit = intersect_cylinder(ray, pos.cy, &hit.dist);
	if (!hit.hit)
		return ((t_hit){false});
	hit.point = translate(ray.origin, ray.dir, hit.dist);
	hit.normal = cylinder_normal(hit.point, ray, pos.cy);
	return (hit);
}
