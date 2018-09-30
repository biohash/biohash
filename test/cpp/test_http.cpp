#include <iostream>

#include "util/test.hpp"

using namespace biohash::test;


TEST(http_0)
{
   CHECK(12 == 13);
   CHECK(12 == 13);
   CHECK(12 == 15);
}

TEST(http_1)
{
    CHECK(12 == 16);
    CHECK(12 == 12);
    CHECK(true);
    CHECK(false);
    CHECK(std::string {"abc"} == std::string {"def"});
}
