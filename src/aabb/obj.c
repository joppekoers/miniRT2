#include "constants.h"

const void* get_pos(const t_gui* gui, const t_obj* obj)
{
	if (obj->shape == SHAPE_TRIANGLE)
		return vec_getp(&gui->triangles, obj->pos_i);
	if (obj->shape == SHAPE_SPHERE)
		return vec_getp(&gui->spheres, obj->pos_i);
	if (obj->shape == SHAPE_PLANE)
		return vec_getp(&gui->planes, obj->pos_i);
	if (obj->shape == SHAPE_CYLINDER)
		return vec_getp(&gui->cylinders, obj->pos_i);
	exit_e("Shape not supported");
}

bool obj_is_bounded(const t_obj* obj)
{
	return obj->shape != SHAPE_PLANE;
}

static t_aabb sphere_aabb(const t_sphere* sp)
{
	const t_vec3 r = vec3(sp->radius, sp->radius, sp->radius);

	return aabb_from_vec3(subtract(sp->origin, r), add(sp->origin, r));
}

// Box around both end caps, grown by the radius in every direction
static t_aabb cylinder_aabb(const t_cylinder* cy)
{
	const t_vec3 r = vec3(cy->radius, cy->radius, cy->radius);
	const t_vec3 min = {
			.x = min2(cy->origin.x, cy->base2.x),
			.y = min2(cy->origin.y, cy->base2.y),
			.z = min2(cy->origin.z, cy->base2.z),
	};
	const t_vec3 max = {
			.x = max2(cy->origin.x, cy->base2.x),
			.y = max2(cy->origin.y, cy->base2.y),
			.z = max2(cy->origin.z, cy->base2.z),
	};

	return aabb_from_vec3(subtract(min, r), add(max, r));
}

static t_aabb triangle_aabb(const t_triangle* tr)
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

	return aabb_from_vec3(min, max);
}

t_aabb obj_get_aabb(const t_gui* gui, const t_obj* obj)
{
	if (obj->shape == SHAPE_TRIANGLE)
		return triangle_aabb(get_pos(gui, obj));
	if (obj->shape == SHAPE_SPHERE)
		return sphere_aabb(get_pos(gui, obj));
	if (obj->shape == SHAPE_CYLINDER)
		return cylinder_aabb(get_pos(gui, obj));
	exit_e("Shape has no aabb");
}

bool obj_is_inside_aabb(const t_gui* gui, const t_obj* obj, const t_aabb* aabb)
{
	const t_aabb obj_aabb = obj_get_aabb(gui, obj);

	return aabb_is_inside(aabb, &obj_aabb.min) && aabb_is_inside(aabb, &obj_aabb.max);
}
