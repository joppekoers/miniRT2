/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   vector.c                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/01/08 15:59:04 by jkoers        #+#    #+#                 */
/*   Updated: 2021/01/18 13:59:11 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "vector.h"
#include "constants.h"
#include <math.h>

inline double length(const t_vec3* v)
{
	return (sqrt(v->x * v->x + v->y * v->y + v->z * v->z));
}

// see doc/project_on_line.png
// a = p
// b = line0
// c = line1

t_vec3 project_on_line(t_vec3 p, t_vec3 line0, t_vec3 line1)
{
	t_vec3 dir;
	t_vec3 v;
	double t;
	t_vec3 p_on_line;

	dir = unit(subtract(line1, line0));
	v = subtract(p, line0);
	t = dot(v, dir);
	p_on_line = translate(line0, dir, t);
	return (p_on_line);
}

t_vec3 vec3(double x, double y, double z)
{
	return (t_vec3){x, y, z};
}
