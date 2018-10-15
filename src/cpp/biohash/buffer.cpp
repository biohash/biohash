#include <stdlib.h>

#include "buffer.hpp"
#include "assert.hpp"

using namespace biohash;

Buffer::Buffer(size_t size)
{
    resize(size);
}

Buffer::~Buffer()
{
    free(data);
}

void Buffer::resize(size_t new_size)
{
    data = static_cast<char*>(realloc(data, new_size));
    ASSERT(data);
    size = new_size;
}
