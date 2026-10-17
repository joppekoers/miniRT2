/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   gui_init.c                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/01/10 14:09:36 by jkoers        #+#    #+#                 */
/*   Updated: 2021/01/12 15:00:02 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "gui.h"
#include "constants.h"
#include "parse_rt.h"
#include "../lib/minilibx-linux/mlx.h"
#include "ray.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

// Create a mlx image of the right size to store the rendered scene

static void set_canvas_for_mlx(t_gui* gui, bool create_window)
{
	const double timer = timer_now();
	gui->mlx = mlx_init();
	if (gui->mlx == NULL)
		exit_e("mlx_init() failed");

	t_canvas* canvas = &gui->canvas;
	if (create_window && MAX_WINDOW_SIZE)
	{
		int screen_x;
		int screen_y;
		mlx_get_screen_size(gui->mlx, &screen_x, &screen_y);
		if (screen_x < (int)gui->x_size)
			gui->x_size = screen_x;
		if (screen_y < (int)gui->y_size)
			gui->y_size = screen_y;
	}
	canvas->mlx_img = mlx_new_image(gui->mlx, (int)gui->x_size, (int)gui->y_size);
	if (canvas->mlx_img == NULL)
		exit_e("Can't create canvas image 1");
	canvas->data = mlx_get_data_addr(canvas->mlx_img, &canvas->bpp, &canvas->line_length, &canvas->byte_order);
	if (canvas->data == NULL)
		exit_e("Can't create canvas image 2");
	timer_print("mlx_init", timer);

	printf("%-12s %zux%zu\n", "resolution", gui->x_size, gui->y_size);
}

static void set_canvas_without_mlx(t_gui* gui)
{
	const unsigned bytes_per_pixel = 3;
	gui->canvas.bpp = 8 * bytes_per_pixel;
	gui->canvas.line_length = gui->x_size * bytes_per_pixel;
	gui->canvas.data = malloc(gui->x_size * gui->y_size * bytes_per_pixel);
}

const u8	 camera_pos_version = 42;

static char* camera_pos_path()
{
	static char path[PATH_MAX] = "";
	if (path[0])
		return path;
	snprintf(path, sizeof path, "%s%s", cache_dir(), "/camera_pos");
	return path;
}

static void save_camera_pos_file(t_camera* camera)
{
	mkdir(cache_dir(), 0755);
	FILE* f = fopen(camera_pos_path(), "wb");
	if (!f)
	{
		printf("Could not store camera pos\n");
		return;
	}
	assert(fwrite(&camera_pos_version, sizeof camera_pos_version, 1, f) == 1);
	camera_serialise(camera, f);
	assert(!fclose(f));
}

// Returns false when there is no usable file
bool read_camera_pos_file(t_camera* camera)
{
	FILE* f = fopen(camera_pos_path(), "rb");
	if (!f)
		return false;
	u8 camera_pos_version_read;
	if (fread(&camera_pos_version_read, sizeof camera_pos_version_read, 1, f) != 1 ||
			camera_pos_version_read != camera_pos_version)
	{
		fclose(f);
		return false;
	}
	camera_deserialise(camera, f);
	assert(!fclose(f));
	return true;
}

static int on_keypress(int keycode, t_gui* gui)
{
	const double d = 4;

	switch (keycode)
	{
		case KEY_ESC:
			exit_success(gui);
			break;
		case KEY_C:
			gui_render_next_camera(gui);
			break;
		case KEY_W:
			camera_updown(&gui->camera, d);
			break;
		case KEY_S:
			camera_updown(&gui->camera, -d);
			break;
		case KEY_A:
			camera_leftright(&gui->camera, -d);
			break;
		case KEY_D:
			camera_leftright(&gui->camera, d);
			break;
		case KEY_F:
			camera_forwardbackward(&gui->camera, d);
			break;
		case KEY_B:
			camera_forwardbackward(&gui->camera, -d);
			break;
		case KEY_UP:
			camera_pitchyaw(&gui->camera, d, 0);
			break;
		case KEY_DOWN:
			camera_pitchyaw(&gui->camera, -d, 0.0);
			break;
		case KEY_LEFT:
			camera_pitchyaw(&gui->camera, 0.0, d);
			break;
		case KEY_RIGHT:
			camera_pitchyaw(&gui->camera, 0.0, -d);
			break;
	}

	gui_display_scene(gui);
	save_camera_pos_file(&gui->camera);

	return (0);
}

// When the red cross/the close button is pressed run this

static int on_cross(t_gui* gui)
{
	exit_success(gui);
	return (0);
}

void open_window(t_gui* gui)
{
	gui->window = mlx_new_window(gui->mlx, (int)gui->x_size, (int)gui->y_size, "miniRT");
	if (gui->window == NULL)
		exit_e("mlx_new_window() failed");
	mlx_key_hook(gui->window, on_keypress, gui);
	if (IS_LINUX)
		mlx_hook(gui->window, 33, 0L, on_cross, gui);
	else
		mlx_hook(gui->window, 17, 1L << 17, on_cross, gui);
	gui_display_scene(gui);
	mlx_loop(gui->mlx);
}

static void parse_3dfile(t_gui* gui, const char* filename)
{
	double timer = timer_now();
	if (str_ends_with(filename, ".rt"))
		parse_rt(gui, filename);
	else if (str_ends_with(filename, ".obj"))
		parse_obj_file(gui, filename);
	else
		exit_e("File type not recognised, expect .rt or .obj;");
	timer_print("Parse file", timer);
}

void gui_init(t_gui* gui, const char* filename, bool create_window)
{
	ft_bzero(gui, sizeof(t_gui));
	gui->spheres = vec(0, sizeof(t_sphere));
	gui->planes = vec(0, sizeof(t_plane));
	gui->cylinders = vec(0, sizeof(t_cylinder));
	gui->triangles = vec(0, sizeof(t_triangle));
	gui->octree = octree_root(0);
	gui->unbounded = vec(0, sizeof(t_obj));
	gui->lights = vec(0, sizeof(t_light));
	gui->cameras = vec(0, sizeof(t_camera));
	if (DEBUG)
		printf("%s\n", filename);

	parse_3dfile(gui, filename);

	if (VERBOSE)
		printf("%-12s %8zu\n", "Objects", gui->octree.objects.length + gui->unbounded.length);

	double timer = timer_now();
	octree_build(gui);
	timer_print("Build octree", timer);

	if (create_window)
	{
		set_canvas_for_mlx(gui, create_window);
		open_window(gui);
	}
	else
		set_canvas_without_mlx(gui);
}
