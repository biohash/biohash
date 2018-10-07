#include <time.h>
#include <stdio.h>

#include "time.hpp"
#include "assert.hpp"


using namespace biohash;

namespace {

int_fast64_t timespec_to_int(struct timespec tp)
{
    return static_cast<int_fast64_t>(tp.tv_sec) * 1000000000
        + static_cast<int_fast64_t>(tp.tv_nsec);
}

}

int_fast64_t time::realtime_now()
{
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return timespec_to_int(tp);
}

int_fast64_t time::monotonic_now()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return timespec_to_int(tp);
}

int time::formatted_now(char* buf, size_t size)
{
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);

    struct tm res;
    localtime_r(&tp.tv_sec, &res);

    int rc = snprintf(buf, size, "%.4d-%.2d-%.2d.%.2d:%.2d:%.2d.%.3ld",
                      res.tm_year + 1900, res.tm_mon, res.tm_mday,
                      res.tm_hour, res.tm_min, res.tm_sec, tp.tv_nsec / 1000000);
    ASSERT(rc == 23);

    return rc;
}
