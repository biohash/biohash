
#include "test_base.hpp"

using namespace biohash;
using namespace biohash::test;

TestBase::TestBase(std::vector<TestBase*>& tests, const char* name,
                   const char* file):
    name {name},
    file {file}
{
    tests.push_back(this);
}

void TestBase::check(Context& test_context, bool cond, long line, const char* text)
{
    if (!cond)
        check_failed(test_context, line, text);
}

void TestBase::check_failed(Context& test_context, long line, const char* text)
{
    Check check {line, text};
    test_context.failed_checks.push_back(check);
}

std::vector<TestBase*>& test::get_default_tests()
{
    static std::vector<TestBase*> tests;
    return tests;
}
