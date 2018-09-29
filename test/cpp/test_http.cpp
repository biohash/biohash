#include <iostream>

#include "util/test.hpp"

using namespace biohash::test;

class TestInstance_http_0: public TestInstance {
public:

    TestInstance_http_0(TestInstances& test_instances, const char* name);
    ~TestInstance_http_0() = default;
    void run(TestContext& test_context) const final override;
};

TestInstance_http_0::TestInstance_http_0(TestInstances& test_instances, const char* name):
    TestInstance(test_instances, name)
{
}

void TestInstance_http_0::run(TestContext& test_context) const
{
    std::cout << "Running abc\n";
}

TestInstance_http_0 test_instance_http_0 {get_default_test_instances(), "http_0"};
