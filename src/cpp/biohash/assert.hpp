#pragma once

#include <stdlib.h>
#include <stdio.h>

#define ASSERT(expression) ((expression) ? static_cast<void>(0): biohash::biohash_assert(#expression, __FILE__, __LINE__))


namespace biohash {

inline void biohash_assert(const char* expression, const char* file, long line)
{
	fprintf(stderr, "Assertion '%s' failed, file '%s' line '%ld'.", expression, file, line);
    abort();
}

}
