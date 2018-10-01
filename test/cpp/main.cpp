#include <iostream>

#include "util/test.hpp"

using namespace biohash::test;

int main(int argc, char** argv)
{
    static_cast<void>(argc);
    static_cast<void>(argv);

    std::vector<TestBase*>& tests = get_default_tests();

    TestRunner::Config config;
    config.prefix = "http_1";

    TestRunner test_runner {tests, config};
    bool success = test_runner.run();
    test_runner.report();
    return success ? 0 : 1;
}
