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

static void log_progress(unsigned int i, const t_gui* gui)
{
	printf("\rProgress %7.3lf%%  ", ((double)i / gui->y_size) * 100.0);
	fflush(stdout);
}

// Claims the next batch of rows [*start, *end) for this thread, false when none are left

static bool find_rows(unsigned int* start, unsigned int* end, const t_thread* thread)
{
	*start = atomic_fetch_add(&thread->gui->row_to_render, thread->rows_to_render);
	if (*start >= thread->gui->y_size)
		return (false);
	*end = *start + thread->rows_to_render;
	if (*end > thread->gui->y_size)
		*end = thread->gui->y_size;
	if (LOG_PROGRESS)
		log_progress(*end, thread->gui);
	return (true);
}

// Render batches of rows until there are none left

static void* run_thread(void* p)
{
	unsigned int x;
	unsigned int y;
	unsigned int end;
	t_thread*	 thread = (t_thread*)p;

	while (find_rows(&y, &end, thread))
	{
		while (y < end)
		{
			x = 0;
			while (x < thread->gui->x_size)
			{
				gui_set_pixel(thread->gui, x, y, compute_color(x, y, thread->gui));
				x++;
			}
			y++;
		}
	}
	return (NULL);
}

static void start_threads(t_thread* threads, t_gui* gui, size_t thread_count)
{
	// About 100 batches per thread keeps the atomic counter traffic negligible
	unsigned int rows_to_render = min2(2, gui->y_size / (unsigned int)(thread_count * 100));
	size_t		 i;

	i = 0;
	while (i < thread_count)
	{
		threads[i].gui = gui;
		threads[i].rows_to_render = rows_to_render;
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

	gui->row_to_render = 0;
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
