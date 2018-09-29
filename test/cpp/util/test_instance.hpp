#pragma once

#include <string>
#include <vector>

#include "test_context.hpp"


namespace biohash {
namespace test {

class TestInstance;

class TestInstances {
public:
    TestInstances() = default;

    void register_test(TestInstance*);

    void run();

    void report();

private:
    std::vector<TestInstance*> m_tests;

    void sort_tests_by_name();
};

TestInstances& get_default_test_instances();



// An abstract base class for test instances. A test instance registers itself
// at construction.
class TestInstance {
public:

    TestInstance(TestInstances& test_instances, const char* name);

    virtual ~TestInstance() = default;

    const char* name;

    virtual void run(TestContext& test_context) const = 0;

private:
    uint64_t m_check_cnt = 0;
};

}
}
