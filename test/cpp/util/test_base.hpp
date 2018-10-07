#pragma once

#include <vector>

#include <biohash/log.hpp>

namespace biohash {
namespace test {

// An abstract base class for test instances. A test instance registers itself
// at construction.
class TestBase {
public:

    TestBase(std::vector<TestBase*>& tests, const char* name, const char* file);

    virtual ~TestBase() = default;

    const char* name;
    const char* file;

    struct Check {
        long line;
        const char* text;
    };

    struct Context {
        log::Logger& logger;
        std::vector<Check> failed_checks;

        Context(log::Logger& logger);
    };

    virtual void run(Context& test_context) const = 0;

    static void check(Context& test_context, bool cond, long line, const char* text);
    static void check_failed(Context& test_context, long line, const char* text);

private:
};

std::vector<TestBase*>& get_default_tests();

}
}
