#include <iostream>
#include <algorithm>
#include <cstring>

#include "test_instance.hpp"

using namespace biohash;
using namespace biohash::test;

// TestInstances

void TestInstances::register_test(TestInstance* instance)
{
    m_tests.push_back(instance);
}

void TestInstances::run()
{
    sort_tests_by_name();
    for (size_t i = 0; i < m_tests.size(); ++i) {
        std::cout << "Running test " << i << ", name = " << m_tests[i]->name << "\n";
        m_tests[i]->run();
    }
}

void TestInstances::sort_tests_by_name()
{
    std::sort(m_tests.begin(), m_tests.end(), [](const TestInstance* a, const TestInstance* b) {
        return std::strcmp(a->name, b->name) < 0;
    });
}

// Default test_instances
TestInstances& test::get_default_test_instances()
{
    static TestInstances test_instances;
    return test_instances;
}

// TestInstance

TestInstance::TestInstance(TestInstances& test_instances, const char* name):
    name {name}
{
    std::cout << "Constructor of TestInstance\n";
    test_instances.register_test(this);
}

void TestInstance::run()
{
    std::cout << "Run, name = " << name << "\n";
}

