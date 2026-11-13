#include "constants.h"
#include "gui.h"
#include "intersect.h"
#include "parse_rt.h"
#include "../../lib/tinyobj/tinyobj_loader_c.h"
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#define OBJ_COLOR ((t_rgb){200, 200, 200})
#define OBJ_X_SIZE 1920
#define OBJ_Y_SIZE 1080
#define OBJ_FOV 80.0

// tinyobj does not free the buffers it gets from the reader and keeps pointing into the .obj buffer while parsing the .mtl
typedef struct s_file_buffers
{
	uint8_t* obj;
	uint8_t* mtl;
} t_file_buffers;

static uint8_t* read_whole_file(const char* path, size_t* size)
{
	FILE*	 f = fopen(path, "rb");
	uint8_t* buf;

	if (f == NULL)
		return NULL;
	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);
	buf = malloc(*size);
	if (fread(buf, 1, *size, f) != *size)
	{
		free(buf);
		buf = NULL;
	}
	fclose(f);
	return buf;
}

static void read_file_callback(void* ctx, const char* path, int is_mtl, const char* obj_path, char** buf, size_t* len)
{
	t_file_buffers* buffers = ctx;
	uint8_t*		data;

	(void)obj_path;
	data = read_whole_file(path, len);
	if (data == NULL)
	{
		printf("Warning: cannot open %s: %s%s\n", path, strerror(errno), is_mtl ? ", using default color" : "");
		*len = 0;
	}
	if (is_mtl)
		buffers->mtl = data;
	else
		buffers->obj = data;
	*buf = (char*)data;
}

static t_rgb unit_to_rgb(const float c[3])
{
	return (t_rgb){
			round(fminf(fmaxf(c[0], 0.0f), 1.0f) * 255.0),
			round(fminf(fmaxf(c[1], 0.0f), 1.0f) * 255.0),
			round(fminf(fmaxf(c[2], 0.0f), 1.0f) * 255.0),
	};
}

static t_vec3 vertex_at(const tinyobj_attrib_t* attrib, int i)
{
	return vec3(attrib->vertices[3 * i], attrib->vertices[3 * i + 1], attrib->vertices[3 * i + 2]);
}

// Faces are triangulated by tinyobj, faces with out of range vertex indices are skipped
static void add_triangles(t_gui* gui, const tinyobj_attrib_t* attrib, const tinyobj_material_t* mats, size_t num_mats)
{
	const tinyobj_vertex_index_t* idx;
	t_obj						  obj;
	t_triangle					  tr;
	int							  mat_id;

	for (unsigned f = 0; f < attrib->num_face_num_verts; f++)
	{
		idx = attrib->faces + 3 * f;
		if (idx[0].v_idx < 0 || idx[1].v_idx < 0 || idx[2].v_idx < 0 ||
				(unsigned)idx[0].v_idx >= attrib->num_vertices || (unsigned)idx[1].v_idx >= attrib->num_vertices ||
				(unsigned)idx[2].v_idx >= attrib->num_vertices)
			continue;
		mat_id = attrib->material_ids[f];
		obj.color = mat_id >= 0 && (size_t)mat_id < num_mats ? unit_to_rgb(mats[mat_id].diffuse) : OBJ_COLOR;
		tr.p0 = vec3f(vertex_at(attrib, idx[0].v_idx));
		tr.p1 = vec3f(vertex_at(attrib, idx[1].v_idx));
		tr.p2 = vec3f(vertex_at(attrib, idx[2].v_idx));
		set_edge_normal(&tr);
		add_parsed_triangle(gui, &obj, &tr);
	}
}

// Above and in front of the model, looking at its average point
static t_camera camera_that_looks_at_scene(const t_gui* gui)
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
	// .obj files follow the OpenGL convention where the viewer is on the +z side
	cam.origin.z = max.z + size / 2.0;
	cam.dir = unit(subtract(avg, cam.origin));
	cam.fov = ft_radians(OBJ_FOV);
	return cam;
}

// An .obj file only has geometry, so the rest of the scene gets defaults
static void set_scene_defaults(t_gui* gui)
{
	if (gui->triangles.length == 0)
		exit_e("No faces found in .obj file");
	gui->x_size = OBJ_X_SIZE;
	gui->y_size = OBJ_Y_SIZE;

	t_camera cam;
	if (!read_camera_pos_file(&cam, gui->render_file_path))
	{
		cam = camera_that_looks_at_scene(gui);
	}
	add_parsed_camera(gui, cam);

	t_light light = {.origin = gui->camera.origin, .brightness = 0.5, .color = {255, 255, 255}};
	vec_push(&gui->lights, &light);

	gui->ambient.brightness = 0.1;
	gui->ambient.color = (t_rgb){255, 255, 255};
	gui->ambient.scalar.r = round(gui->ambient.color.r * gui->ambient.brightness);
	gui->ambient.scalar.g = round(gui->ambient.color.g * gui->ambient.brightness);
	gui->ambient.scalar.b = round(gui->ambient.color.b * gui->ambient.brightness);
}

void parse_obj_file(t_gui* gui, const char* path)
{
	tinyobj_attrib_t	attrib;
	tinyobj_shape_t*	shapes = NULL;
	size_t				num_shapes = 0;
	tinyobj_material_t* tmats = NULL;
	size_t				num_mats = 0;
	t_file_buffers		buffers = {NULL, NULL};
	int					ret;

	ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &tmats, &num_mats, path, read_file_callback, &buffers,
			TINYOBJ_FLAG_TRIANGULATE);
	free(buffers.obj);
	free(buffers.mtl);
	if (ret != TINYOBJ_SUCCESS)
	{
		char msg[1024];
		snprintf(msg, sizeof(msg), "Cannot parse %s (tinyobj error %d)", path, ret);
		exit_e(msg);
	}
	add_triangles(gui, &attrib, tmats, num_mats);
	tinyobj_attrib_free(&attrib);
	tinyobj_shapes_free(shapes, num_shapes);
	tinyobj_materials_free(tmats, num_mats);
	set_scene_defaults(gui);
}
