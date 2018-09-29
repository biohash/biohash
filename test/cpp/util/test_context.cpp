#include "test_context.hpp"

#include <biohash/assert.hpp>

using namespace biohash;
using namespace biohash::test;


TestContext::TestContext()
{
}

void TestContext::thread_begin(std::thread::id tid)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    BIOHASH_ASSERT(m_threads.find(tid) == m_threads.end());
    m_threads[tid] = ++m_thread_counter;
}

void TestContext::thread_end(std::thread::id tid)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    BIOHASH_ASSERT(m_threads.find(tid) != m_threads.end());
    m_threads[tid] = ++m_thread_counter;
}
