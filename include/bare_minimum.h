#pragma once
#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>
#include <assert.h>
#include <printf.h>
#include <stdlib.h>

#define DOUBLE_MAX 9999999999.0
#define DOUBLE_MIN -9999999999.0
#define EPSILON (1e-7)

#ifdef linux
#define IS_LINUX 1
#else
#define IS_LINUX 0
#endif

void  exit_e(const char* msg) __attribute__((__noreturn__));
void* malloc_safe(size_t size);
void* calloc_safe(size_t size);

#define min2(a, b) ((a) < (b)) ? (a) : (b)
#define max2(a, b) ((a) > (b)) ? (a) : (b)

#define min3(a, b, c) ((a) < (b)) ? (((a) < (c)) ? (a) : (c)) : (((b) < (c)) ? (b) : (c))
#define max3(a, b, c) ((a) > (b)) ? (((a) > (c)) ? (a) : (c)) : (((b) > (c)) ? (b) : (c))

#define DEBUG 1
#define debug_assert(x) (DEBUG) ? assert(x) : (x)

// crude macro TODO: improve
#define EXIT_WITH_ERROR(...) \
	do \
	{ \
		fprintf(stderr, "Function: "); \
		fprintf(stderr, __func__); \
		fprintf(stderr, "(...)\nFile:     ./"); \
		fprintf(stderr, __FILE__); \
		fprintf(stderr, ":"); \
		fprintf(stderr, "%d", __LINE__); \
		fprintf(stderr, "\nMessage:  "); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		exit(EXIT_FAILURE); \
	} while (0)
