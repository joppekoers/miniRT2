#include "constants.h"

t_aabb obj_get_aabb(const t_obj* obj)
{
	if (obj->shape == SHAPE_TRIANGLE)
	{
		const t_triangle* tr = &obj->pos.tr;
		const t_vec3	  min = {
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
	};

	exit_e("Shape not supported");
}

bool obj_is_inside_aabb(const t_obj* obj, const t_aabb* aabb)
{
	if (obj->shape == SHAPE_TRIANGLE)
	{
		const t_triangle* tr = &obj->pos.tr;
		return aabb_is_inside(aabb, &tr->p0) && //
			   aabb_is_inside(aabb, &tr->p1) && //
			   aabb_is_inside(aabb, &tr->p2);
	}
	exit_e("Shape not supported");
}
