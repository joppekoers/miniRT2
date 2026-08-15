#include "constants.h"
#include <sys/time.h>
#include <stdio.h>

// seconds since an arbitrary fixed point, only useful for measuring durations
double timer_now(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)tv.tv_usec / 1e6;
}

void timer_print(const char* label, double start)
{
	printf("%-12s %8.3f seconds\n", label, timer_now() - start);
}
