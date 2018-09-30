#include <iostream>

#include "util/test.hpp"

using namespace biohash::test;

int main(int argc, char** argv)
{
    static_cast<void>(argc);
    static_cast<void>(argv);

    std::vector<TestBase*>& tests = get_default_tests();
    TestRunner test_runner {tests};
    bool success = test_runner.run();
    test_runner.report();
    return success ? 0 : 1;
}
