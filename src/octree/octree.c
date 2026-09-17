#include "constants.h"

#include "intersect.h"
#include "vector.h"
#include "marena.h"

#define OCTREE_MAX_DEPTH 10

void* malloc2(size_t n)
{
	static t_marena arena = {.p = 0};
	if (!arena.p)
		marena_init(&arena, 20 * 1000 * 1000);
	return marena_malloc(&arena, n);
}

t_octree octree(t_aabb aabb, size_t objects)
{
	t_octree octree;
	ft_bzero(&octree, sizeof(t_octree));
	octree.aabb = aabb;
	octree.objects = vecm(objects, sizeof(t_obj), &malloc2, NULL);
	return octree;
}

// To init the root of a octree, only use on initial creation
t_octree octree_root(size_t objects)
{
	const t_vec3 min = {
			.x = -DBL_MAX,
			.y = -DBL_MAX,
			.z = -DBL_MAX,
	};
	const t_vec3 max = {
			.x = DBL_MAX,
			.y = DBL_MAX,
			.z = DBL_MAX,
	};
	return octree(aabb_from_vec3(min, max), objects);
}

// depth of the deepest leaf, a tree of only a root has depth 1
static size_t octree_max_depth(const t_octree* o)
{
	size_t	  max_depth = 0;
	size_t	  i = 0;
	t_octree* child;

	while ((child = vec_getp(&o->children, i++)))
	{
		const size_t d = octree_max_depth(child);
		if (d > max_depth)
			max_depth = d;
	}
	return max_depth + 1;
}

t_octree octree_from_objects(const t_vec* objects)
{
	t_octree root = octree_root(objects->length);
	t_obj*	 obj;
	size_t	 i = 0;
	while ((obj = vec_getp(objects, i++)))
	{
		debug_assert(octree_add_obj(&root, obj));
	}
	// The root octree node is infinitely large, so subdividing would do nothing without shrinking first
	octree_shrink_to_fit(&root);

	octree_subdivide(&root);
	if (DEBUG)
		printf("Octree depth: %zu\n", octree_max_depth(&root));
	return root;
}

// Returns true if it fits inside the octree's aabb
bool octree_add_obj(t_octree* octree, t_obj* obj)
{
	if (!obj_is_inside_aabb(obj, &octree->aabb))
		return false;
	vec_push(&octree->objects, obj);

	return true;
}

void octree_shrink_to_fit(t_octree* octree)
{
	if (octree->objects.length == 0)
		return;

	t_vec3 min = {.x = DBL_MAX, .y = DBL_MAX, .z = DBL_MAX};
	t_vec3 max = {.x = -DBL_MAX, .y = -DBL_MAX, .z = -DBL_MAX};

	size_t i = 0;
	t_obj* obj;
	while ((obj = vec_getp(&octree->objects, i++)))
	{
		const t_aabb aabb_obj = obj_get_aabb(obj);

		min.x = min2(min.x, aabb_obj.min.x);
		min.y = min2(min.y, aabb_obj.min.y);
		min.z = min2(min.z, aabb_obj.min.z);

		max.x = max2(max.x, aabb_obj.max.x);
		max.y = max2(max.y, aabb_obj.max.y);
		max.z = max2(max.z, aabb_obj.max.z);
	}
	octree->aabb.min = min;
	octree->aabb.max = max;
}

static void octree_bounce_children(const t_octree* octree, const t_ray* ray, t_bounce* bounce);

static void octree_bounce_recurse(const t_octree* octree, const t_ray* ray, t_bounce* bounce)
{
	const double aabb_intersect = aabb_intersects(&octree->aabb, ray);
	if (aabb_intersect < 0 || aabb_intersect > bounce->distance)
		return;

	t_obj* obj;
	size_t i = 0;
	while ((obj = vec_getp(&octree->objects, i++)))
	{
		t_hit hit = hit_obj(obj->shape, obj->pos, *ray);
		if (hit.hit && hit.dist < bounce->distance)
			bounce_assign(bounce, &hit, obj);
	}

	octree_bounce_children(octree, ray, bounce);
}

static void octree_bounce_children(const t_octree* octree, const t_ray* ray, t_bounce* bounce)
{
	t_octree* child;
	size_t	  i = 0;
	while ((child = vec_getp(&octree->children, i++)))
		octree_bounce_recurse(child, ray, bounce);
}

t_bounce octree_bounce(const t_octree* octree, const t_ray* ray)
{
	t_bounce bounce = {.obj = NULL, .distance = DBL_MAX, .ray_origin = ray->origin};
	octree_bounce_recurse(octree, ray, &bounce);
	return bounce;
}

// creates 8 children for the octree, subdividing them equally
void make_children(t_octree* o)
{
	const t_vec3 min = o->aabb.min;
	const t_vec3 max = o->aabb.max;
	const t_vec3 center = scale(add(min, max), 0.5);
	const size_t objects = o->objects.length / 8;
	t_octree new;

	o->children = vec(8, sizeof(t_octree));
	new = octree(aabb_from_vec3(min, center), objects);
	vec_push(&o->children, &new);
	new = octree(aabb_from_vec3(vec3(center.x, min.y, min.z), vec3(max.x, center.y, center.z)), objects);
	vec_push(&o->children, &new);
	new = octree(aabb_from_vec3(vec3(min.x, center.y, min.z), vec3(center.x, max.y, center.z)), objects);
	vec_push(&o->children, &new);
	new = octree(aabb_from_vec3(vec3(center.x, center.y, min.z), vec3(max.x, max.y, center.z)), objects);
	vec_push(&o->children, &new);
	new = octree(aabb_from_vec3(vec3(min.x, min.y, center.z), vec3(center.x, center.y, max.z)), objects);
	vec_push(&o->children, &new);
	new = octree(aabb_from_vec3(vec3(center.x, min.y, center.z), vec3(max.x, center.y, max.z)), objects);
	vec_push(&o->children, &new);
	new = octree(aabb_from_vec3(vec3(min.x, center.y, center.z), vec3(center.x, max.y, max.z)), objects);
	vec_push(&o->children, &new);
	new = octree(aabb_from_vec3(center, max), objects);
	vec_push(&o->children, &new);
}

void move_objects_to_children(t_octree* o)
{
	t_vec*	  objects = &o->objects;
	size_t	  i = objects->length;
	t_obj*	  obj;
	t_octree* child;

	while (i && (obj = vec_getp(objects, --i)))
	{
		size_t _i = 0;
		while ((child = vec_getp(&o->children, _i++)))
		{
			if (!obj_is_inside_aabb(obj, &child->aabb))
				continue;

			vec_push(&child->objects, obj);

			// the current obj is inserted into a child, so it should be removed from the current aabb
			// we take the last obj that we have and set that at the current index
			// this last element will always get popped
			if (i + 1 != objects->length && objects->length)
			{
				void* last = vec_getp(objects, objects->length - 1);
				vec_set(objects, i, last);
			}
			vec_pop(objects, NULL);
			break;
		}
	}
}

static void subdivide_recursive(t_octree* o, size_t depth)
{
	if (o->objects.length < 100 || depth >= OCTREE_MAX_DEPTH)
		return;

	make_children(o);
	move_objects_to_children(o);

	t_octree* child;
	size_t	  i = 0;
	while ((child = vec_getp(&o->children, i++)))
	{
		subdivide_recursive(child, depth + 1);
	}
}

// build the entire tree, subdivide the children all the way down
void octree_subdivide(t_octree* o)
{
	subdivide_recursive(o, 0);
}
