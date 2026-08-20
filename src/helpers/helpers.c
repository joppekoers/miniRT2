#include <stdarg.h>
#include <stdio.h>
#include "../../lib/libft/include/libft.h"

double min_double(int n, ...)
{
	va_list ap;
	int		i;
	double	min;
	double	value;

	va_start(ap, n);
	min = va_arg(ap, double);
	for (i = 1; i < n; i++)
	{
		value = va_arg(ap, double);
		if (value < min)
			min = value;
	}
	va_end(ap);
	return min;
}

double max_double(int n, ...)
{
	va_list ap;
	int		i;
	double	max;
	double	value;

	va_start(ap, n);
	max = va_arg(ap, double);
	for (i = 1; i < n; i++)
	{
		value = va_arg(ap, double);
		if (value > max)
			max = value;
	}
	va_end(ap);
	return max;
}

bool str_ends_with(const char* s, const char* end)
{
	size_t s_length = ft_strlen(s);
	size_t end_length = ft_strlen(end);

	if (end_length > s_length)
		return false;
	while (end_length && s[--s_length] == end[end_length - 1])
		end_length--;
	return end_length == 0;
}
