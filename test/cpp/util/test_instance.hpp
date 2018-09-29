#pragma once

#include <string>
#include <vector>

namespace biohash {
namespace test {

class TestInstance;

class TestInstances {
public:
    TestInstances() = default;

    void register_test(TestInstance*);

    void run();

private:
    std::vector<TestInstance*> m_tests;

    void sort_tests_by_name();
};

TestInstances& get_default_test_instances();



// An individual test instance. A test instance registers itself at
// construction.
class TestInstance {
public:

    TestInstance(TestInstances& test_instances, const char* name);

    const char* name;

    void run();

private:

};

}
}
