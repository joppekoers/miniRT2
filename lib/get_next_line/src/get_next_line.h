/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.h                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: jkoers <jkoers@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/12/01 13:29:56 by jkoers        #+#    #+#                 */
/*   Updated: 2020/12/14 20:25:36 by jkoers        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif
# include <stddef.h>
# include <unistd.h>

int					get_next_line(int fd, char **line);

typedef struct		s_buf
{
	char			*data;
	ssize_t			size;
	ssize_t			start;
	struct s_buf	*next;
}					t_buf;

void				*ft_memcpy(void *dest, void *src, size_t n);
void				shift(t_buf **fd);
t_buf				*new_buf(size_t size);

#endif
