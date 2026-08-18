/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.c                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/12/01 13:31:41 by jkoers        #+#    #+#                 */
/*   Updated: 2020/12/22 23:00:31 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdbool.h>

static int	error(t_buf **fd)
{
	if (fd == NULL)
		return (-1);
	while (*fd != NULL)
		shift(fd);
	return (-1);
}

static bool	found_end(t_buf *fd, size_t *total_size)
{
	ssize_t	i;

	i = 0;
	while (i < fd->size)
	{
		if (fd->data[i + fd->start] == '\n')
			return (true);
		*total_size += 1;
		i++;
	}
	return (false);
}

static int	join(t_buf **fd, size_t total_size, char **line)
{
	size_t	i;

	i = 0;
	*line = malloc((size_t)(total_size + 1));
	if (*line == NULL)
		return (error(fd));
	(*line)[total_size] = '\0';
	while ((*fd)->next != NULL)
	{
		ft_memcpy(*line + i, (*fd)->data + (*fd)->start, (size_t)((*fd)->size));
		i += (size_t)((*fd)->size);
		shift(fd);
	}
	ft_memcpy(*line + i, (*fd)->data + (*fd)->start, total_size - i);
	(*fd)->start += (total_size - i) + 1;
	(*fd)->size -= (total_size - i) + 1;
	if ((*fd)->size < 0)
		shift(fd);
	return (*fd ? 1 : 0);
}

int			ft_get_next_line(const int fd, char **line)
{
	static t_buf	*fds[FD_SETSIZE];
	t_buf			*cur;
	size_t			total_size;

	if (fd < 0 || BUFFER_SIZE <= 0 || line == NULL)
		return (error(fd < 0 ? NULL : &fds[fd]));
	total_size = 0;
	if (!fds[fd])
		fds[fd] = new_buf(0);
	else if (found_end(fds[fd], &total_size))
		return (join(&fds[fd], total_size, line));
	cur = fds[fd];
	while (cur)
	{
		cur->next = new_buf(BUFFER_SIZE);
		if (cur->next == NULL)
			return (error(&fds[fd]));
		cur = cur->next;
		cur->size = read(fd, cur->data, BUFFER_SIZE);
		if (cur->size == -1)
			return (error(&fds[fd]));
		if (found_end(cur, &total_size) || cur->size == 0)
			return (join(&fds[fd], total_size, line));
	}
	return (error(&fds[fd]));
}
