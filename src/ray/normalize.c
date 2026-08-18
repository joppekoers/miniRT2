/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   normalize.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/13 01:01:31 by jkoers        #+#    #+#                 */
/*   Updated: 2021/02/13 01:01:31 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "vector.h"
#include "constants.h"
#include <math.h>

t_vec3 unit(t_vec3 v)
{
	normalize(&v);
	return (v);
}
