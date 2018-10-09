#pragma once

#define TEST(NAME)\
class TestBase_##NAME: public TestBase {\
public:\
\
    TestBase_##NAME();\
    ~TestBase_##NAME() = default;\
    void run(Context& test_context) const final override;\
}; \
\
TestBase_##NAME::TestBase_##NAME():\
    TestBase(get_default_tests(), #NAME, __FILE__)\
{\
}; \
\
TestBase_##NAME test_##NAME {}; \
void TestBase_##NAME::run(Context& test_context) const


#define CHECK(EXP) TestBase::check(test_context, EXP, __LINE__, #EXP)

#define CHECK_EQUAL(A, B) TestBase::check_equal(test_context, A, B, __LINE__)
