/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parse_rule_shapes.c                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/12/27 20:55:30 by jkoers        #+#    #+#                 */
/*   Updated: 2021/01/12 14:15:10 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "parse_rt.h"
#include "../lib/libft/include/libft.h"
#include "gui.h"
#include "constants.h"
#include "parse_rt.h"
#include "vector.h"
#include "intersect.h"
#include <stdlib.h>

static void push_shape(t_gui* gui, t_obj* obj, t_vec* positions, void* pos)
{
	obj->pos_i = positions->length;
	if (vec_push(positions, pos) == NULL || vec_push(&gui->shapes, obj) == NULL)
		exit_e("malloc");
}

void add_sphere(t_gui* gui, char* line)
{
	t_obj	 obj;
	t_sphere sp;
	char**	 items;

	obj.shape = SHAPE_SPHERE;
	items = split_clamp(line, 4);
	set_point(&sp.origin, items[1]);
	sp.radius = strtodbl_clamp(items[2], '\0', 0.0, DOUBLE_MAX) * 0.5;
	sp.radius2 = sp.radius * sp.radius;
	set_color(&obj.color, items[3]);
	ft_free_until_null_char(items);
	push_shape(gui, &obj, &gui->spheres, &sp);
}

void add_plane(t_gui* gui, char* line)
{
	t_obj	obj;
	t_plane pl;
	char**	items;

	obj.shape = SHAPE_PLANE;
	items = split_clamp(line, 4);
	set_point(&pl.origin, items[1]);
	set_dir(&pl.normal, items[2]);
	set_color(&obj.color, items[3]);
	ft_free_until_null_char(items);
	push_shape(gui, &obj, &gui->planes, &pl);
}

void add_cylinder(t_gui* gui, char* line)
{
	t_obj	   obj;
	t_cylinder cy;
	char**	   items;

	obj.shape = SHAPE_CYLINDER;
	items = split_clamp(line, 6);
	set_point(&cy.origin, items[1]);
	set_dir(&cy.dir, items[2]);
	cy.radius = strtodbl_clamp(items[3], '\0', 0.0, DOUBLE_MAX) * 0.5;
	cy.height = strtodbl_clamp(items[4], '\0', 0.0, DOUBLE_MAX);
	set_color(&obj.color, items[5]);
	cy.base2 = translate(cy.origin, cy.dir, cy.height);
	ft_free_until_null_char(items);
	push_shape(gui, &obj, &gui->cylinders, &cy);
}

void add_parsed_triangle(t_gui* gui, t_obj* obj, t_triangle* tr)
{
	obj->shape = SHAPE_TRIANGLE;
	push_shape(gui, obj, &gui->triangles, tr);
}

void add_triangle(t_gui* gui, char* line)
{
	t_obj	   obj;
	t_triangle tr;
	char**	   items;

	items = split_clamp(line, 5);
	set_point(&tr.p0, items[1]);
	set_point(&tr.p1, items[2]);
	set_point(&tr.p2, items[3]);
	set_color(&obj.color, items[4]);
	ft_free_until_null_char(items);
	set_edge_normal(&tr);
	add_parsed_triangle(gui, &obj, &tr);
}
