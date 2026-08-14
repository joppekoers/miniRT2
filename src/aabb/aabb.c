#include "constants.h"
#include <float.h>
#include "intersect.h"

bool aabb_is_inside(const t_aabb* aabb, const t_vec3* p)
{
	return (p->x >= aabb->min.x && p->x <= aabb->max.x) && //
		   (p->y >= aabb->min.y && p->y <= aabb->max.y) && //
		   (p->z >= aabb->min.z && p->z <= aabb->max.z);
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
	octree->children_count = 0;
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

	// TODO: loop though children
	// TODO: this is not ignoring the aabbs that are further away than the closest_dist, that would never hit
	// for (uint8_t i = 0; i++; i < octree->children_count)
	// {
	// 	t_bounce bounce = octree_bounce(octree->children[i], ray);
	// 	if (bounce.obj && bounce.color < closest_dist)
	// 	{
	// 		closest_dist = bounce.dist;
	// 		bounce.obj = obj;
	// 		bounce.color = obj->color;
	// 		bounce.point = bounce.point;
	// 		bounce.normal = bounce.normal;
	// 	}
	// }

	return bounce;
}

bool octree_subdivide(t_octree* octree);
