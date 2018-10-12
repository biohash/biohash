#include <biohash/buffer.hpp>

#include "util/test.hpp"

using namespace biohash;
using namespace biohash::test;

TEST(buffer)
{
    const size_t size_1 = 3;
    const size_t size_2 = 700;
    Buffer buf(size_1);
    CHECK_EQUAL(buf.size, size_1);
    memcpy(buf.data, "abc", 3);
    buf.resize(size_2);
    CHECK_EQUAL(buf.size, size_2);
    CHECK_EQUAL(memcmp(buf.data, "abc", 3), 0);
    buf.resize(0);
}
