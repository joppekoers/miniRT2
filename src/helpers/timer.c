#include "constants.h"
#include <sys/time.h>
#include <stdio.h>

double timer_now()
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)tv.tv_usec / 1e6;
}

void timer_print(const char* label, double start)
{
	if (VERBOSE)
		printf("%-12s %8.3fs\n", label, timer_now() - start);
}
