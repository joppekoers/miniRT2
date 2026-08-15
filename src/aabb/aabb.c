#include "constants.h"
#include <float.h>
#include "intersect.h"
#include "vector.h"

bool aabb_is_inside(const t_aabb* aabb, const t_vec3* p)
{
	return (p->x >= aabb->min.x && p->x <= aabb->max.x) && //
		   (p->y >= aabb->min.y && p->y <= aabb->max.y) && //
		   (p->z >= aabb->min.z && p->z <= aabb->max.z);
}

t_aabb aabb_vec(t_vec3 min, t_vec3 max)
{
	return (t_aabb){.min = min, .max = max};
}
t_aabb aabb_double(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z)
{
	return (t_aabb){//
			.min = (t_vec3){.x = min_x, .y = min_y, .z = min_z}, //
			.max = (t_vec3){.x = max_x, .y = max_y, .z = max_z}};
}

bool aabb_intersects(const t_aabb* aabb, const t_ray* ray)
{
	double tmin, tmax, tymin, tymax, tzmin, tzmax;
	if (ray->dir.x >= 0)
	{
		tmin = (aabb->min.x - ray->origin.x) / ray->dir.x;
		tmax = (aabb->max.x - ray->origin.x) / ray->dir.x;
	}
	else
	{
		tmin = (aabb->max.x - ray->origin.x) / ray->dir.x;
		tmax = (aabb->min.x - ray->origin.x) / ray->dir.x;
	}
	if (ray->dir.y >= 0)
	{
		tymin = (aabb->min.y - ray->origin.y) / ray->dir.y;
		tymax = (aabb->max.y - ray->origin.y) / ray->dir.y;
	}
	else
	{
		tymin = (aabb->max.y - ray->origin.y) / ray->dir.y;
		tymax = (aabb->min.y - ray->origin.y) / ray->dir.y;
	}
	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	if (ray->dir.z >= 0)
	{
		tzmin = (aabb->min.z - ray->origin.z) / ray->dir.z;
		tzmax = (aabb->max.z - ray->origin.z) / ray->dir.z;
	}
	else
	{
		tzmin = (aabb->max.z - ray->origin.z) / ray->dir.z;
		tzmax = (aabb->min.z - ray->origin.z) / ray->dir.z;
	}
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return tmax > 0;
}

// To init the root of a octree, only use on initial creation
void octree_root(t_octree* octree)
{
	ft_bzero(octree, sizeof(t_octree));
	octree->aabb.min = (t_vec3){
			.x = -DBL_MAX,
			.y = -DBL_MAX,
			.z = -DBL_MAX,
	};
	octree->aabb.max = (t_vec3){
			.x = DBL_MAX,
			.y = DBL_MAX,
			.z = DBL_MAX,
	};
	//
	octree->objects = ft_arr(10000);
}

// Returns true if it fits inside the octree's aabb
bool octree_add_obj(t_octree* octree, t_obj* obj)
{
	if (!obj_is_inside_aabb(obj, &octree->aabb))
		return false;
	ft_arr_push(&octree->objects, obj);

	return true;
}

void octree_shrink_to_fit(t_octree* octree)
{
	t_vec3 min = octree->aabb.min;
	t_vec3 max = octree->aabb.max;

	size_t i = 0;
	t_obj* obj;
	while ((obj = ft_arr_get(octree->objects, i++)))
	{
		const t_aabb aabb_obj = obj_get_aabb(obj);

		min.x = max2(min.x, aabb_obj.min.x);
		min.y = max2(min.y, aabb_obj.min.y);
		min.z = max2(min.z, aabb_obj.min.z);

		max.x = min2(max.x, aabb_obj.max.x);
		max.y = min2(max.y, aabb_obj.max.y);
		max.z = min2(max.z, aabb_obj.max.z);
	}
	octree->aabb.min = max;
	octree->aabb.max = min;
}

t_bounce octree_bounce(const t_octree* octree, const t_ray* ray)
{
	if (!aabb_intersects(&octree->aabb, ray))
		return (t_bounce){.obj = NULL};

	double	 closest_dist = DBL_MAX;
	t_bounce bounce;
	t_obj*	 obj;
	size_t	 i = 0;

	bounce.obj = NULL;
	bounce.ray_origin = ray->origin;

	while ((obj = ft_arr_get(octree->objects, i++)))
	{
		t_hit hit = hit_obj(obj->shape, obj->pos, *ray);
		if (hit.hit && hit.dist < closest_dist)
		{
			closest_dist = hit.dist;
			bounce.obj = obj;
			bounce.color = obj->color;
			bounce.point = hit.point;
			bounce.normal = hit.normal;
		}
	}

	// TODO: this is not ignoring the aabbs that are further away than the closest_dist, that would never hit
	// for (uint8_t i = 0; i++; i < octree->children_count)
	// {
	// 	t_bounce b = octree_bounce(octree->children[i], ray);
	// 	double	 d = distance(ray->origin, b.point);
	// 	if (b.obj && d < closest_dist)
	// 		bounce = b;
	// }

	return bounce;
}

// build the entire tree, subdivide the children all the way down
bool octree_subdivide(t_octree* octree)
{
	(void)octree;
	return true;
	// t_arr* objects = octree->objects;

	// if (objects->length < 100)
	// 	return false;

	// const t_vec3 min = octree->aabb.min;
	// const t_vec3 max = octree->aabb.max;
	// const t_vec3 center = scale(add(min, max), 0.5);

	// octree->children[0]->aabb = aabb_vec(min, center);
	// octree->children[1]->aabb = aabb_vec(vec3(center.x, min.y, min.z), vec3(max.x, center.y, center.z));
	// octree->children[2]->aabb = aabb_vec(vec3(min.x, center.y, min.z), vec3(center.x, max.y, center.z));
	// octree->children[3]->aabb = aabb_vec(vec3(center.x, center.y, min.z), vec3(max.x, max.y, center.z));
	// octree->children[4]->aabb = aabb_vec(vec3(min.x, min.y, center.z), vec3(center.x, center.y, max.z));
	// octree->children[5]->aabb = aabb_vec(vec3(center.x, min.y, center.z), vec3(max.x, center.y, max.z));
	// octree->children[6]->aabb = aabb_vec(vec3(min.x, center.y, center.z), vec3(center.x, max.y, max.z));
	// octree->children[7]->aabb = aabb_vec(center, max);
	// octree->children_count = 8;
	// sizeof (t_octree)

	// size_t i = objects->length;
	// t_obj* obj;
	// while (i && (obj = ft_arr_get(objects, --i)))
	// {
	// 	for (size_t child_i = 0; child_i++; child_i < octree->children_count)
	// 	{
	// 		const child = octree->children[child_i];

	// 		if (!obj_is_inside_aabb(obj, child))
	// 			continue;

	// 		ft_arr_push(&child, obj);

	// 		// the current obj is inserted into a child, so it should be removed from the current aabb
	// 		// we take the last obj that we have and set that at the current index
	// 		// this last element will always get popped
	// 		if (i + 1 != objects->length && objects->length)
	// 		{
	// 			void* last = ft_arr_get(&objects, objects->length - 1);
	// 			ft_arr_set(&objects, i, last);
	// 		}
	// 		ft_arr_pop(&objects, NULL);
	// 	}
	// }
	// for (size_t child_i = 0; child_i++; child_i < octree->children_count)
	// {
	// 	const child = octree->children[child_i];
	// 	octree_subdivide(child);
	// }
}
