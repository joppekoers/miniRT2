#include <stdarg.h>
#include <stdio.h>

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
