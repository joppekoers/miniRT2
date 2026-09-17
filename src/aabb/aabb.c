#include "constants.h"
#include <float.h>
#include <math.h>

bool aabb_is_inside(const t_aabb* aabb, const t_vec3* p)
{
	return (p->x >= aabb->min.x && p->x <= aabb->max.x) && //
		   (p->y >= aabb->min.y && p->y <= aabb->max.y) && //
		   (p->z >= aabb->min.z && p->z <= aabb->max.z);
}

t_aabb aabb_from_vec3(t_vec3 min, t_vec3 max)
{
	return (t_aabb){.min = min, .max = max};
}
t_aabb aabb_double(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z)
{
	return (t_aabb){//
			.min = (t_vec3){.x = min_x, .y = min_y, .z = min_z}, //
			.max = (t_vec3){.x = max_x, .y = max_y, .z = max_z}};
}

// 1.0 / -0.0 = -inf keeps t1/t2 ordered by fmin/fmax, and fmin/fmax return
// the non-NaN operand when 0 * inf occurs (origin exactly on a slab plane),
// so the test stays conservative instead of poisoning the interval
static void slab(double bmin, double bmax, double origin, double dir, double* tmin, double* tmax)
{
	const double inv = 1.0 / dir;
	const double t1 = (bmin - origin) * inv;
	const double t2 = (bmax - origin) * inv;

	*tmin = fmax(*tmin, fmin(t1, t2));
	*tmax = fmin(*tmax, fmax(t1, t2));
}

// distance along the ray to the box entry point, 0 when the origin is
// inside the box, -1 when the ray misses
double aabb_intersects(const t_aabb* aabb, const t_ray* ray)
{
	double tmin = -DBL_MAX;
	double tmax = DBL_MAX;

	slab(aabb->min.x, aabb->max.x, ray->origin.x, ray->dir.x, &tmin, &tmax);
	slab(aabb->min.y, aabb->max.y, ray->origin.y, ray->dir.y, &tmin, &tmax);
	slab(aabb->min.z, aabb->max.z, ray->origin.z, ray->dir.z, &tmin, &tmax);
	if (tmax < tmin || tmax <= 0)
		return -1;
	return fmax(tmin, 0.0);
}
