#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "bare_minimum.h"
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

const char* cache_dir()
{
	static char dir[PATH_MAX] = "";

	// if previously loaded return early
	if (dir[0])
		return dir;

	const char* home = getenv("HOME");
	const char* base = "/tmp";
	const char* suffix = "";
#ifdef linux
	const char* xdg = getenv("XDG_CACHE_HOME");
	if (xdg && *xdg)
		base = xdg;
	else if (home && *home)
	{
		base = home;
		suffix = "/.cache";
	}
#else
	if (home && *home)
	{
		base = home;
		suffix = "/Library/Caches";
	}
#endif
	snprintf(dir, sizeof dir, "%s%s/minirt2", base, suffix);
	return (dir);
}

size_t ft_fwrite(const void* ptr, size_t size, FILE* stream)
{
	assert(fwrite(ptr, size, 1, stream) == 1);
	return size;
}

size_t ft_fread(void* ptr, size_t size, FILE* stream)
{
	assert(fread(ptr, size, 1, stream) == 1);
	return size;
}

void strreplace(char* s, const char search, const char replace)
{
	if (search == '\0')
		return;
	while (*s)
	{
		if (*s == search)
			*s = replace;
		s++;
	}
}
