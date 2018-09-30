#pragma once

#include <vector>
#include <mutex>

#include "test_base.hpp"

namespace biohash {
namespace test {

// The TestRunner manages the entire run of the test suite. It gets the list of
// tests and some options as input. At TestRunner::run(), the TestRunner
// creates a number of threads that execute the tests. The TestRunner keeps
// track of all failures. When run() completes, the tests are done and the
// worker threads are joined. After run(), success() indicates whether there
// were failures, and report writes more detailed information about the test
// results to std::cerr.
class TestRunner {
public:
    TestRunner(const std::vector<TestBase*>& tests);

    bool run();
    void report();

private:
    const std::vector<TestBase*>& m_tests;

    // The mutex protects m_test_ndx.
    std::mutex m_mutex;
    size_t m_test_ndx = 0;

    struct TestResult {
        size_t thread_ndx;
        const char* name;
        const char* file;
        std::vector<TestBase::Check> failed_checks;
    };

    std::vector<TestResult> m_test_results;

    void run_worker(size_t thread_ndx);
};

}
}
