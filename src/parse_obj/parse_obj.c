#include "get_next_line.h"
#include "constants.h"
#include "gui.h"
#include "intersect.h"
#include "parse_rt.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#define OBJ_COLOR ((t_rgb){200, 200, 200})
#define OBJ_X_SIZE 1920
#define OBJ_Y_SIZE 1080
#define OBJ_FOV 80.0

typedef struct s_obj_file
{
	t_vec  vertices; // t_vec3
	t_gui* gui;
} t_obj_file;

static const char* skip_space(const char* s)
{
	while (ft_isspace(*s))
		s++;
	return s;
}

// parses [+-]digits[.digits][eE[+-]digits], returns the char after the number
static const char* read_double(const char* s, double* num)
{
	size_t i;
	long   exp;

	i = s[0] == '+' || s[0] == '-';
	if (!ft_isdigit(s[i]))
		exit_char(s[i], '0');
	while (ft_isdigit(s[i]))
		i++;
	if (s[i] == '.')
		i++;
	while (ft_isdigit(s[i]))
		i++;
	*num = ft_strtodbl((char*)s);
	if (s[i] != 'e' && s[i] != 'E')
		return s + i;
	s += i + 1;
	i = s[0] == '+' || s[0] == '-';
	if (!ft_isdigit(s[i]))
		exit_char(s[i], '0');
	while (ft_isdigit(s[i]))
		i++;
	exp = ft_strtonum((char*)s);
	while (exp > 0 && exp--)
		*num *= 10.0;
	while (exp < 0 && exp++)
		*num /= 10.0;
	return s + i;
}

// parses [+-]digits, returns the char after the number
static const char* read_long(const char* s, long* num)
{
	size_t i;

	i = s[0] == '+' || s[0] == '-';
	if (!ft_isdigit(s[i]))
		exit_char(s[i], '0');
	while (ft_isdigit(s[i]))
		i++;
	*num = ft_strtonum((char*)s);
	return s + i;
}

// reads one face element "v", "v/vt", "v/vt/vn" or "v//vn",
// stores the referenced vertex in *p
static const char* read_index(const t_obj_file* file, const char* s, t_vec3* p)
{
	long raw;
	long i;

	s = read_long(s, &raw);
	if (raw < 0)
		i = raw + (long)file->vertices.length;
	else
		i = raw - 1;
	if (i < 0 || i >= (long)file->vertices.length)
		exit_range(raw, 1, (long)file->vertices.length);
	debug_assert(vec_get(&file->vertices, p, (size_t)i));
	while (*s != '\0' && !ft_isspace(*s))
		s++;
	return s;
}

static void push_triangle(t_gui* gui, t_vec3 p0, t_vec3 p1, t_vec3 p2)
{
	t_obj	   obj;
	t_triangle tr;

	obj.color = OBJ_COLOR;
	tr.p0 = p0;
	tr.p1 = p1;
	tr.p2 = p2;
	set_edge_normal(&tr);
	add_parsed_triangle(gui, &obj, &tr);
}

// faces with more than 3 vertices are converted to a triangle fan
static void read_line_face(t_obj_file* file, const char* s)
{
	t_vec3 p0;
	t_vec3 prev;
	t_vec3 cur;

	s = read_index(file, skip_space(s), &p0);
	s = read_index(file, skip_space(s), &prev);
	s = skip_space(s);
	if (*s == '\0')
		exit_e("A face needs at least 3 vertices");
	while (*s != '\0')
	{
		s = read_index(file, s, &cur);
		push_triangle(file->gui, p0, prev, cur);
		prev = cur;
		s = skip_space(s);
	}
}

static void read_line_vertex(t_obj_file* file, const char* s)
{
	t_vec3 vertex;

	s = read_double(skip_space(s), &vertex.x);
	s = read_double(skip_space(s), &vertex.y);
	s = read_double(skip_space(s), &vertex.z);
	if (*skip_space(s) != '\0')
		exit_e("Expected end of line after 3 vertex coordinates");
	if (vec_push(&file->vertices, &vertex) == NULL)
		exit_e("malloc");
}

static void print_object_name(const char* s)
{
	size_t len;

	len = ft_strlen((char*)s);
	while (len > 0 && ft_isspace(s[len - 1]))
		len--;
	printf("Object: %.*s\n", (int)len, s);
}

static void read_line(t_obj_file* file, const char* line)
{
	const char* s;

	s = skip_space(line);
	if (s[0] == 'v' && ft_isspace(s[1]))
		read_line_vertex(file, s + 1);
	else if (s[0] == 'f' && ft_isspace(s[1]))
		read_line_face(file, s + 1);
	else if ((s[0] == 'o' || s[0] == 'g') && ft_isspace(s[1]))
		print_object_name(skip_space(s + 1));
}

// Camera placement from minirt_rs look_at(): above and in front of the model, looking at its average point
static void add_look_at_camera(t_gui* gui)
{
	t_vec3		avg = {0.0, 0.0, 0.0};
	t_vec3		min = {DBL_MAX, DBL_MAX, DBL_MAX};
	t_vec3		max = {-DBL_MAX, -DBL_MAX, -DBL_MAX};
	t_triangle* tr;
	size_t		i = 0;
	t_camera	cam;

	while ((tr = vec_getp(&gui->triangles, i++)))
	{
		avg = add(avg, scale(add(add(tr->p0, tr->p1), tr->p2), 1.0 / 3.0));
		min.x = min2(min.x, min3(tr->p0.x, tr->p1.x, tr->p2.x));
		min.y = min2(min.y, min3(tr->p0.y, tr->p1.y, tr->p2.y));
		min.z = min2(min.z, min3(tr->p0.z, tr->p1.z, tr->p2.z));
		max.x = max2(max.x, max3(tr->p0.x, tr->p1.x, tr->p2.x));
		max.y = max2(max.y, max3(tr->p0.y, tr->p1.y, tr->p2.y));
		max.z = max2(max.z, max3(tr->p0.z, tr->p1.z, tr->p2.z));
	}
	avg = scale(avg, 1.0 / gui->triangles.length);
	const t_vec3 diagonal = subtract(max, min);
	const double size = length(&diagonal);

	cam.origin = add(avg, vec3(size / 5.0, size / 5.0, size / 5.0));
	cam.origin.z = -fabs(avg.z + size / 2.0);
	cam.dir = unit(subtract(avg, cam.origin));
	cam.fov = ft_radians(OBJ_FOV);
	add_parsed_camera(gui, cam);
}

// An .obj file only has geometry, so the rest of the scene gets defaults
static void set_scene_defaults(t_gui* gui)
{
	if (gui->triangles.length == 0)
		exit_e("No faces found in .obj file");
	gui->x_size = OBJ_X_SIZE;
	gui->y_size = OBJ_Y_SIZE;
	add_look_at_camera(gui);

	t_light light = {.origin = gui->camera.origin, .brightness = 0.5, .color = {255, 255, 255}};
	if (vec_push(&gui->lights, &light) == NULL)
		exit_e("malloc");

	gui->ambient.brightness = 0.1;
	gui->ambient.color = (t_rgb){255, 255, 255};
	gui->ambient.scalar.r = round(gui->ambient.color.r * gui->ambient.brightness);
	gui->ambient.scalar.g = round(gui->ambient.color.g * gui->ambient.brightness);
	gui->ambient.scalar.b = round(gui->ambient.color.b * gui->ambient.brightness);
}

void parse_obj_file(t_gui* gui, const char* path)
{
	const int  fd = open(path, O_RDONLY);
	t_obj_file file;
	char*	   line;

	if (fd < 0)
	{
		char msg[1024];
		snprintf(msg, sizeof(msg), "Cannot open %s: %s", path, strerror(errno));
		exit_e(msg);
	}
	file.vertices = vec(1000, sizeof(t_vec3));
	file.gui = gui;
	while (ft_get_next_line(fd, &line) > 0)
	{
		line_error(line);
		read_line(&file, line);
		free(line);
	}
	line_error("");
	vec_free(&file.vertices, NULL);
	set_scene_defaults(gui);
}
