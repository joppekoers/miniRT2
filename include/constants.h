/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   constants.h                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/12/21 19:27:08 by jkoers        #+#    #+#                 */
/*   Updated: 2021/01/12 14:18:13 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "../lib/libft/include/libft.h"
#include "../settings.h"
#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>
#include <assert.h>

#define DOUBLE_MAX 9999999999.0
#define DOUBLE_MIN -9999999999.0
#define EPSILON (1e-7)

#ifdef linux
#define IS_LINUX 1
#define KEY_ESC 65307
#define NEXT_CAMERA_KEY 99
#else
#define IS_LINUX 0
#define KEY_ESC 53
#define NEXT_CAMERA_KEY 8
#endif

void   exit_e(const char* msg) __attribute__((__noreturn__));
void   exit_range(long num, long min, long max);
void   exit_ranged(double num, double min, double max);
void   exit_char(char got, char expected);
void*  malloc_safe(size_t size);
void*  calloc_safe(size_t size);
double max_double(int n, ...);
double min_double(int n, ...);
void   test();

int	   get_number_of_threads();

#define min2(a, b) ((a) < (b)) ? (a) : (b)
#define max2(a, b) ((a) > (b)) ? (a) : (b)

#define min3(a, b, c) ((a) < (b)) ? (((a) < (c)) ? (a) : (c)) : (((b) < (c)) ? (b) : (c))
#define max3(a, b, c) ((a) > (b)) ? (((a) > (c)) ? (a) : (c)) : (((b) > (c)) ? (b) : (c))

#define DEBUG 1
#define debug_assert(x) \
	if (DEBUG) \
	{ \
		assert(x); \
	};

typedef enum e_shape
{
	SHAPE_SPHERE,
	SHAPE_PLANE,
	SHAPE_SQUARE,
	SHAPE_CYLINDER,
	SHAPE_TRIANGLE,
	SHAPE_LAST
} t_shape;

typedef enum e_rule
{
	RULE_SPHERE,
	RULE_PLANE,
	RULE_SQUARE,
	RULE_CYLINDER,
	RULE_TRIANGLE,
	RULE_RESOLUTION,
	RULE_AMBIENT,
	RULE_CAMERA,
	RULE_LIGHT,
	RULE_LAST
} t_rule;

char* rule_id(t_rule i);
char* rule_name(t_rule i);
char* line_error(char* line);

typedef struct s_rgb
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} t_rgb;

typedef struct s_vec3
{
	double x;
	double y;
	double z;
} t_vec3;

typedef struct s_ray
{
	t_vec3 origin;
	t_vec3 dir;
} t_ray;

typedef struct s_canvas
{
	void* mlx_img;
	char* data;
	int	  bpp;
	int	  line_length;
	int	  byte_order;
} t_canvas;

typedef struct s_ambient
{
	double brightness;
	t_rgb  color;
	t_rgb  scalar;
} t_ambient;

typedef struct s_camera
{
	t_vec3 origin;
	t_vec3 dir;
	double fov;
} t_camera;

typedef struct s_light
{
	t_vec3 origin;
	double brightness;
	t_rgb  color;
} t_light;

typedef struct s_aabb
{
	t_vec3 min;
	t_vec3 max;

} t_aabb;
typedef struct s_octree
{
	struct s_octree* children[8];
	uint8_t			 children_count;
	t_aabb			 aabb;
	t_arr*			 objects;

} t_octree;

typedef struct s_gui
{
	// holds all the objects in the scene
	t_arr* shapes;

#ifdef USE_OCTREE
	t_octree octree;
#endif
	t_arr*		 lights;
	t_arr*		 cameras;
	size_t		 camera_i;
	t_ambient	 ambient;
	unsigned int x_size;
	unsigned int y_size;
	t_camera	 camera;
	unsigned int row_to_render;

	void*		 mlx;
	void*		 window;
	t_canvas	 canvas;
} t_gui;

typedef struct s_cylinder
{
	t_vec3 origin;
	t_vec3 base2;
	t_vec3 dir;
	double radius;
	double height;
} t_cylinder;

typedef struct s_plane
{
	t_vec3 origin;
	t_vec3 normal;
} t_plane;

typedef struct s_sphere
{
	t_vec3 origin;
	double radius;
	double radius2;
} t_sphere;

typedef struct s_triangle
{
	t_vec3 p0;
	t_vec3 p1;
	t_vec3 p2;
	t_vec3 edge1;
	t_vec3 edge2;
	t_vec3 normal;
} t_triangle;

typedef union u_pos
{
	t_cylinder cy;
	t_plane	   pl;
	t_sphere   sp;
	t_triangle tr;
} t_pos;

typedef struct s_obj
{
	t_shape shape;
	t_rgb	color;
	t_pos	pos;
} t_obj;

typedef struct s_hit
{
	bool   hit;
	double dist;
	t_vec3 point;
	t_vec3 normal;
} t_hit;

typedef struct s_bounce
{
	t_obj* obj;
	t_rgb  color;
	t_vec3 point;
	t_vec3 normal;
	t_vec3 ray_origin;
} t_bounce;

bool	 aabb_is_inside(const t_aabb* aabb, const t_vec3* p);
bool	 aabb_intersects(const t_aabb* aabb, const t_ray* ray);

t_aabb	 obj_get_aabb(const t_obj* obj);
bool	 obj_is_inside_aabb(const t_obj* obj, const t_aabb* aabb);

void	 octree_root(t_octree* octree);
bool	 octree_subdivide(t_octree* octree);
bool	 octree_add_obj(t_octree* octree, t_obj* obj);
void	 octree_shrink_to_fit(t_octree* octree);
t_bounce octree_bounce(const t_octree* octree, const t_ray* ray);

#endif
