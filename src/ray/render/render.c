/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   render_bonus.c                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/01/05 13:18:30 by jkoers        #+#    #+#                 */
/*   Updated: 2021/01/18 13:21:18 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "constants.h"
#include "ray.h"
#include "gui.h"
#include <pthread.h>
#include <math.h>
#include <stdio.h>

static size_t pixel_count(const t_gui* gui)
{
	return (gui->x_size * gui->y_size);
}

static void log_progress(size_t rendered_up_to, const t_gui* gui)
{
	printf("\rProgress %7.3lf%%  ", ((double)rendered_up_to / pixel_count(gui)) * 100.0);
	fflush(stdout);
}

// Claims the next batch of pixel indexes [*start, *end) for this thread, false when none are left.
// A pixel index runs row by row: index = y * x_size + x.

static bool find_pixels(size_t* start, size_t* end, const t_thread* thread)
{
	const size_t total = pixel_count(thread->gui);

	*start = atomic_fetch_add(&thread->gui->pixel_to_render, thread->pixels_to_render);
	if (*start >= total)
		return (false);
	*end = *start + thread->pixels_to_render;
	if (*end > total)
		*end = total;
	if (LOG_PROGRESS)
		log_progress(*end, thread->gui);
	return (true);
}

// Render batches of pixels until there are none left

static void* run_thread(void* p)
{
	size_t	  i;
	size_t	  end;
	size_t	  x;
	size_t	  y;
	t_thread* thread = (t_thread*)p;

	while (find_pixels(&i, &end, thread))
	{
		x = i % thread->gui->x_size;
		y = i / thread->gui->x_size;
		while (i < end)
		{
			gui_set_pixel(thread->gui, x, y, compute_color(x, y, thread->gui));
			x++;
			if (x == thread->gui->x_size)
			{
				x = 0;
				y++;
			}
			i++;
		}
	}
	return (NULL);
}

static void start_threads(t_thread* threads, t_gui* gui, size_t thread_count)
{
	size_t i;

	i = 0;
	while (i < thread_count)
	{
		threads[i].gui = gui;
		threads[i].pixels_to_render = max2(1, pixel_count(gui) / 1000); // limits the number of locks to 1000 per render
		pthread_create(&threads[i].id, NULL, run_thread, (void*)(&threads[i]));
		i++;
	}
}

void render(t_gui* gui)
{
	const double timer = timer_now();
	const size_t thread_count = (size_t)get_number_of_threads();
	t_thread	 threads[thread_count];
	size_t		 i;

	gui->pixel_to_render = 0;
	octree_sort_nearest(&gui->octree, &gui->camera.origin);
	start_threads(threads, gui, thread_count);
	i = 0;
	while (i < thread_count)
	{
		pthread_join(threads[i].id, NULL);
		i++;
	}
	if (LOG_PROGRESS)
		printf("\rProgress %7.3lf%%  \n", 100.0);
	timer_print("Render", timer);
}
