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
#include "bare_minimum.h"
#include "vector.h"

#ifdef linux
#define KEY_ESC 65307
#define NEXT_CAMERA_KEY 99
#else
#define KEY_ESC 53
#define NEXT_CAMERA_KEY 8
#endif

void   exit_range(long num, long min, long max);
void   exit_ranged(double num, double min, double max);
void   exit_char(char got, char expected);
double max_double(int n, ...);
double min_double(int n, ...);
void   test();

bool   str_ends_with(const char* s, const char* end);

int	   get_number_of_threads();

double timer_now(void);
void   timer_print(const char* label, double start);

typedef enum __attribute__((__packed__)) e_shape
{
	SHAPE_SPHERE,
	SHAPE_PLANE,
	SHAPE_SQUARE,
	SHAPE_CYLINDER,
	SHAPE_TRIANGLE,
	SHAPE_LAST
} t_shape;

typedef enum __attribute__((__packed__)) e_rule
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
	t_vec  children; // type: t_octree
	t_aabb aabb;
	t_vec  objects; // type: t_obj
} t_octree;

typedef struct s_gui
{
	// holds all the objects in the scene
	t_vec shapes; // type: t_obj

#ifdef USE_OCTREE
	t_octree octree;
#endif
	t_vec		 lights; // type: t_light
	t_vec		 cameras; // type: t_camera
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
#ifdef PRE_COMPUTE_TRIANGLE
	t_vec3 edge1;
	t_vec3 edge2;
	t_vec3 normal;
#endif
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
	// The index of the position array;
	// u32	  pos_i;
	t_pos pos;
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
	const t_obj* obj;
	t_rgb		 color;
	t_vec3		 point;
	t_vec3		 normal;
	t_vec3		 ray_origin;
	double		 distance;
} t_bounce;

void	 bounce_assign(t_bounce* b, const t_hit* hit, const t_obj* obj);
void	 bounce_nobounce(t_bounce* b);

bool	 aabb_is_inside(const t_aabb* aabb, const t_vec3* p);
double	 aabb_intersects(const t_aabb* aabb, const t_ray* ray);
t_aabb	 aabb_from_vec3(t_vec3 min, t_vec3 max);
t_aabb	 aabb_double(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z);

t_aabb	 obj_get_aabb(const t_obj* obj);
bool	 obj_is_inside_aabb(const t_obj* obj, const t_aabb* aabb);

t_octree octree(t_aabb aabb, size_t objects);
t_octree octree_root(size_t objects);
t_octree octree_from_objects(const t_vec* objects);
void	 octree_subdivide(t_octree* octree);
bool	 octree_add_obj(t_octree* octree, t_obj* obj);
void	 octree_shrink_to_fit(t_octree* octree);
t_bounce octree_bounce(const t_octree* octree, const t_ray* ray);

#endif
