#include "constants.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdarg.h>

#define STAT_FORMAT "%-12s %10s"

double timer_now()
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)tv.tv_usec / 1e6;
}

void print_stat(const char* label, const char* fmt, ...)
{
	char	value[64];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(value, sizeof value, fmt, ap);
	va_end(ap);
	printf(STAT_FORMAT "\n", label, value);
}

void timer_print(const char* label, double start)
{
	if (VERBOSE)
		print_stat(label, "%.3fs", timer_now() - start);
}
