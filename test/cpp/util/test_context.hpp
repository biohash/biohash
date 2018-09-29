#pragma once

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

namespace biohash {
namespace test {

class TestContext {
public:
    TestContext();

    void thread_begin(std::thread::id tid);
    void thread_end(std::thread::id tid);

    void test_begin();

    void check(bool cond, const char* file, long line, const char* cond_text,
               const char* test_name);

private:
    std::mutex m_mutex;
    uintmax_t m_thread_counter = 0;
    std::map<std::thread::id,  uintmax_t> m_threads;



};

}
}
