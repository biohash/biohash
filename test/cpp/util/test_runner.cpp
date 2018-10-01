#include <iostream>
#include <thread>

#include "test_runner.hpp"

#include <biohash/assert.hpp>

using namespace biohash;
using namespace biohash::test;


TestRunner::TestRunner(const std::vector<TestBase*>& tests, const Config& config):
    m_tests {tests},
    m_config {config},
    m_indices {calculate_indices(tests, config)}
{
    std::cerr << "m_indices.size() = " << m_indices.size() << "\n";

    m_test_results.resize(m_indices.size());
}

bool TestRunner::run()
{
    int nthreads = 4;
    std::vector<std::thread> threads;

    for (int i = 0; i < nthreads; ++i)
        threads.emplace_back(&TestRunner::run_worker, this, i);

    for (std::thread& t: threads)
       t.join();

    ASSERT(m_next_ndx == m_indices.size());

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
            ndx = m_next_ndx;
            ASSERT(ndx <= m_indices.size());
            if (ndx == m_indices.size())
                break;
            else
                ++m_next_ndx;
        }

        TestBase* test = m_tests[m_indices[ndx]];
        
        TestBase::Context context;
        test->run(context);

        TestResult& result = m_test_results[ndx];
        result.thread_ndx = thread_ndx;
        result.name = test->name;
        result.file = test->file;
        result.failed_checks = context.failed_checks;
    }
}

std::vector<size_t> TestRunner::calculate_indices(const std::vector<TestBase*>& tests,
                                                  const Config& config)
{
    std::vector<size_t> indices;
    for (size_t i = 0; i < tests.size(); ++i) {
        bool keep = std::strncmp(config.prefix.data(), tests[i]->name, config.prefix.size()) == 0;

        if (keep)
            indices.push_back(i);
    }

    // sort_tests_by_name();
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        return std::strcmp(tests[a]->name, tests[b]->name) < 0;
    });

    return indices;
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
