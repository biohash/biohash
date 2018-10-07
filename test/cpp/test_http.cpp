#include <iostream>

#include "util/test.hpp"

using namespace biohash;
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

TEST(http_2)
{
    log::Logger& logger = test_context.logger;
    logger.fatal("Hello fatal, str = %s", "Interesting");
    logger.info("Hello info, number = %d", 123);
    logger.trace("Hello trace");

}
