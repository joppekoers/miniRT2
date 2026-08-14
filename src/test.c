#include <assert.h>
#include "constants.h"

void test()
{

	assert(min3(-1.0f, 0.0f, 1.0f) == -1.0f);
	assert(min3(-0.0f, -1.0f, 0.0f) == -1.0f);
	assert(min3(1.0f, 0.0f, -1.0f) == -1.0f);

	assert(max3(-1.0f, 0.0f, 1.0f) == 1.0f);
	assert(max3(-0.0f, 1.0f, 0.0f) == 1.0f);
	assert(max3(1.0f, 0.0f, -1.0f) == 1.0f);
}
