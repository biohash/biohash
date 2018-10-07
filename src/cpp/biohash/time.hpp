#pragma once

#include <stdint.h>

namespace biohash {
namespace time {

    // The system's real time since the epoch.
    int_fast64_t realtime_now();

    // Returns current time since an arbitray point using a monotonic clock.
    int_fast64_t monotonic_now();

    int formatted_now(char* buf, size_t size);
}
}
