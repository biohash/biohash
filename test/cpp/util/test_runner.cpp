#include <iostream>
#include <thread>

#include "test_runner.hpp"

#include <biohash/assert.hpp>

using namespace biohash;
using namespace biohash::test;


TestRunner::TestRunner(const std::vector<TestBase*>& tests):
    m_tests {tests}
{
    m_test_results.resize(tests.size());
}

bool TestRunner::run()
{
//    sort_tests_by_name();
//{
//    std::sort(m_tests.begin(), m_tests.end(), [](const TestInstance* a, const TestInstance* b) {
//        return std::strcmp(a->name, b->name) < 0;
//    });
//}


    int nthreads = 4;
    std::vector<std::thread> threads;

    for (int i = 0; i < nthreads; ++i)
        threads.emplace_back(&TestRunner::run_worker, this, i);

    for (std::thread& t: threads)
       t.join();

    ASSERT(m_test_ndx == m_tests.size());

    bool success = true;
    for (const TestResult& result: m_test_results) {
        if (!result.failed_checks.empty()) {
            success = false;
            break;
        }
    }

    return success;
}

void TestRunner::run_worker(size_t thread_ndx)
{
    while (true) {
        size_t ndx;
        {
            // Acquire the next test index.
            std::lock_guard<std::mutex> lock {m_mutex};
            ndx = m_test_ndx;
            ASSERT(ndx <= m_tests.size());
            if (ndx == m_tests.size())
                break;
            else
                ++m_test_ndx;
        }

        TestBase* test = m_tests[ndx];
        
        TestBase::Context context;
        test->run(context);

        TestResult& result = m_test_results[ndx];
        result.thread_ndx = thread_ndx;
        result.name = test->name;
        result.file = test->file;
        result.failed_checks = context.failed_checks;
    }
}

void TestRunner::report()
{
    size_t failed_tests_cnt = 0;
    for (const TestResult& result: m_test_results) {
        if (!result.failed_checks.empty())
            ++failed_tests_cnt;
    }

    std::cerr 
        << "Number of completed tests: " << m_test_results.size() << "\n"
        << "Number of failed tests: " << failed_tests_cnt << "\n";
    for (const TestResult& result: m_test_results) {
        if (result.failed_checks.empty())
            continue;
        std::cerr << "Test failed in thread " << result.thread_ndx << ": '"
            << result.name << "' in file '" << result.file << "'\n";
        for (const TestBase::Check& check: result.failed_checks)
            std::cerr << "  Check failed: line " << check.line << ", '" << check.text << "'\n";
    }
}
