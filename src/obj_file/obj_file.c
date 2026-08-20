#include "get_next_line.h"
#include "constants.h"
#include "gui.h"
#include "intersect.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define OBJ_COLOR ((t_rgb){200, 200, 200})

typedef struct s_obj_file
{
	t_vec  vertices; // t_vec3
	t_vec* shapes; // t_obj
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

static void push_triangle(t_vec* shapes, t_vec3 p0, t_vec3 p1, t_vec3 p2)
{
	t_obj obj;

	obj.shape = SHAPE_TRIANGLE;
	obj.color = OBJ_COLOR;
	obj.pos.tr.p0 = p0;
	obj.pos.tr.p1 = p1;
	obj.pos.tr.p2 = p2;
	set_edge_normal(&obj.pos.tr);
	debug_assert(vec_push(shapes, &obj));
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
		push_triangle(file->shapes, p0, prev, cur);
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

void parse_obj_file(t_gui* gui, const char* path)
{
	const int  fd = open(path, O_RDONLY);
	t_obj_file file;
	char*	   line;

	if (fd < 0)
		exit_e(ft_strjoin("Cannot open ", path));
	file.vertices = vec(1000, sizeof(t_vec3));
	file.shapes = &gui->shapes;
	while (ft_get_next_line(fd, &line) > 0)
	{
		line_error(line);
		read_line(&file, line);
		free(line);
	}
	line_error("");
	vec_free(&file.vertices, NULL);
}
