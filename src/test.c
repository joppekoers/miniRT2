#include <assert.h>
#include "constants.h"
#include "vector.h"

void test()
{
	assert(min3(-1.0f, 0.0f, 1.0f) == -1.0f);
	assert(min3(-0.0f, -1.0f, 0.0f) == -1.0f);
	assert(min3(1.0f, 0.0f, -1.0f) == -1.0f);

	assert(max3(-1.0f, 0.0f, 1.0f) == 1.0f);
	assert(max3(-0.0f, 1.0f, 0.0f) == 1.0f);
	assert(max3(1.0f, 0.0f, -1.0f) == 1.0f);

	{
		t_vec v = vec(10, sizeof(long));
		long  count = 100;
		for (long i = 0; i < count; i++)
			vec_push(&v, &i);

		for (long i = 0; i < count; i++)
		{
			long out;
			assert(vec_get(&v, &out, i));
			assert(out == i);
		}
	}
	{
		t_vec	v = vec(10, sizeof(uint8_t));
		uint8_t count = 100;
		for (uint8_t i = 0; i < count; i++)
			vec_push(&v, &i);

		for (uint8_t i = 0; i < count; i++)
		{
			uint8_t out;
			assert(vec_get(&v, &out, i));
			assert(out == i);
		}
	}
	{
		t_vec v = vec(1, sizeof(uint8_t));
		for (uint8_t i = 1; i < 11; i++)
			vec_push(&v, &i);

		for (uint8_t i = 2; i < 11; i++)
			vec_pop(&v, NULL);

		uint8_t out;
		assert(vec_get(&v, &out, 0));
		assert(out == 1);

		assert(v.length == 1);
	}
	{
		t_vec v = vec(1, sizeof(uint8_t));
		for (uint8_t i = 1; i <= 10; i++)
			vec_push(&v, &i);

		for (uint8_t i = 0; i < 9; i++)
			vec_shift(&v, NULL);

		uint8_t out;
		assert(vec_get(&v, &out, 0));
		assert(out == 10);
		assert(v.length == 1);
	}
	{
		assert(str_ends_with("a", "a"));
		assert(!str_ends_with("ab", "a"));
		assert(!str_ends_with("a", "ab"));
		assert(!str_ends_with("a", "b"));
		assert(str_ends_with("aab", "ab"));
	}
}
