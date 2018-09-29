#include <iostream>

#include "util/test.hpp"

using namespace biohash::test;

int main(int argc, char** argv)
{
    std::cout << "start of main()\n";

    biohash::test::get_default_test_instances().run();

    std::cout << "Tests are done\n";
    return 0;
}
