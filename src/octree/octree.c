#include "constants.h"

#include "intersect.h"
#include "vector.h"

#define OCTREE_MAX_DEPTH 10
// An obj that overlaps more children than this stays in the parent, so big objects are not copied everywhere
#define OCTREE_MAX_COPIES 4

t_octree octree(t_aabb aabb, size_t objects)
{
	t_octree octree;
	ft_bzero(&octree, sizeof(t_octree));
	octree.aabb = aabb;
	octree.objects = vec(objects, sizeof(t_obj));
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

// The parser puts all bounded objects in the root, this subdivides them over the tree
void octree_build(t_gui* gui)
{
	// The root octree node is infinitely large, so subdividing would do nothing without shrinking first
	octree_shrink_to_fit(gui, &gui->octree);

	octree_subdivide(gui, &gui->octree);
	if (DEBUG)
		printf("Octree depth: %zu\n", octree_max_depth(&gui->octree));
}

void octree_shrink_to_fit(const t_gui* gui, t_octree* octree)
{
	if (octree->objects.length == 0)
		return;

	t_vec3 min = {.x = DBL_MAX, .y = DBL_MAX, .z = DBL_MAX};
	t_vec3 max = {.x = -DBL_MAX, .y = -DBL_MAX, .z = -DBL_MAX};

	size_t i = 0;
	t_obj* obj;
	while ((obj = vec_getp(&octree->objects, i++)))
	{
		const t_aabb aabb_obj = obj_get_aabb(gui, obj);

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

// Everything about the ray that stays the same during one traversal
typedef struct s_traverse
{
	const t_gui* gui;
	const t_ray* ray;
	t_vec3		 inv_dir;
	double		 stop_d;
} t_traverse;

static void octree_bounce_children(const t_traverse* t, const t_octree* octree, t_bounce* bounce);

static void octree_bounce_recurse(const t_traverse* t, const t_octree* octree, t_bounce* bounce)
{
	const double aabb_d = aabb_intersects(&octree->aabb, &t->ray->origin, &t->inv_dir);
	if (aabb_d < 0 || aabb_d > bounce->distance)
		return;

	t_obj* obj;
	double dist;
	size_t i = 0;
	while ((obj = vec_getp(&octree->objects, i++)))
	{
		if (hit_obj(t->gui, obj, *t->ray, &dist) && dist < bounce->distance)
		{
			bounce_assign(bounce, dist, obj);
			if (t->stop_d > 0)
				return;
		}
	}

	octree_bounce_children(t, octree, bounce);
}

static void octree_bounce_children(const t_traverse* t, const t_octree* octree, t_bounce* bounce)
{
	t_octree* child;
	size_t	  i = 0;
	while ((child = vec_getp(&octree->children, i++)))
	{
		octree_bounce_recurse(t, child, bounce);
		if (bounce->obj && t->stop_d > 0)
			return;
	}
}

// stop_d: if > 0 stop at first obj that is closer than that d
t_bounce octree_bounce(const t_gui* gui, const t_ray* ray, double stop_d)
{
	const t_traverse t = {
			.gui = gui,
			.ray = ray,
			.inv_dir = {1.0 / ray->dir.x, 1.0 / ray->dir.y, 1.0 / ray->dir.z},
			.stop_d = stop_d,
	};
	double	 d = stop_d <= 0 ? DBL_MAX : stop_d;
	t_bounce bounce = {.obj = NULL, .distance = d, .ray_origin = ray->origin};
	t_obj*	 obj;
	double	 dist;
	size_t	 i = 0;

	// Hitting these first gives a shorter bounce distance, so more octree nodes get skipped
	while ((obj = vec_getp(&gui->unbounded, i++)))
	{
		if (hit_obj(gui, obj, *ray, &dist) && dist < bounce.distance)
			bounce_assign(&bounce, dist, obj);
	}

	if (stop_d > 0 && bounce.obj)
		return bounce;

	octree_bounce_recurse(&t, &gui->octree, &bounce);
	return bounce;
}

t_bounce octree_bounce_closest(const t_gui* gui, const t_ray* ray)
{
	t_bounce bounce = octree_bounce(gui, ray, -1);

	if (bounce.obj)
	{
		bounce.point = translate(ray->origin, ray->dir, bounce.distance);
		bounce.normal = normal_obj(gui, bounce.obj, bounce.point, *ray);
	}
	return bounce;
}

t_bounce octree_bounce_first(const t_gui* gui, const t_ray* ray, const t_vec3 p)
{
	return octree_bounce(gui, ray, distance(&ray->origin, &p) - 1e-4);
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

void move_objects_to_children(const t_gui* gui, t_octree* o)
{
	t_vec*	  objects = &o->objects;
	size_t	  i = objects->length;
	t_obj*	  obj;
	t_octree* child;

	while (i && (obj = vec_getp(objects, --i)))
	{
		const t_aabb aabb = obj_get_aabb(gui, obj);
		size_t		 copies = 0;
		size_t		 _i = 0;
		while ((child = vec_getp(&o->children, _i++)))
			copies += aabb_overlaps(&aabb, &child->aabb);
		if (copies > OCTREE_MAX_COPIES)
			continue;

		// An obj on the border of children goes in all of them, rays hitting it twice give the same distance
		_i = 0;
		while ((child = vec_getp(&o->children, _i++)))
		{
			if (aabb_overlaps(&aabb, &child->aabb))
				vec_push(&child->objects, obj);
		}

		// the current obj is inserted into children, so it should be removed from the current aabb
		// we take the last obj that we have and set that at the current index
		// this last element will always get popped
		if (i + 1 != objects->length && objects->length)
		{
			void* last = vec_getp(objects, objects->length - 1);
			vec_set(objects, i, last);
		}
		vec_pop(objects, NULL);
	}
}

static void subdivide_recursive(const t_gui* gui, t_octree* o, size_t depth)
{
	if (o->objects.length < 100 || depth >= OCTREE_MAX_DEPTH)
	{
		vec_shrink_to_fit(&o->objects);
		return;
	}

	make_children(o);
	move_objects_to_children(gui, o);
	// Most objects moved to the children, so most of the capacity is unused now
	vec_shrink_to_fit(&o->objects);

	t_octree* child;
	size_t	  i = 0;
	while ((child = vec_getp(&o->children, i++)))
	{
		subdivide_recursive(gui, child, depth + 1);
	}
}

// build the entire tree, subdivide the children all the way down
void octree_subdivide(const t_gui* gui, t_octree* o)
{
	subdivide_recursive(gui, o, 0);
}
