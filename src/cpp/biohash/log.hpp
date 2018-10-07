#pragma once

#include <string>
#include <cstdio>
#include <stdarg.h>

namespace biohash {
namespace log {

enum class Level {
    off,
    fatal,
    error,
    warn,
    info,
    debug,
    trace
};

const char* level_to_string(Level level);
bool string_to_level(const char* str, Level& level);

class Sink {
public:
    virtual void put(const char* data, size_t size) = 0;
};

class NullSink: public Sink {
public:
    NullSink() = default;
    void put(const char* data, size_t size) override;
};

class StderrSink: public Sink {
public:
    StderrSink() = default;
    void put(const char* data, size_t size) override;
};

class Logger {
public:
    // The logger will only log messages with a level less than or equal to
    // threshold.
    Logger(const std::string& id, Sink& sink, Level threshold);

    void fatal(const char* format, ...);
    void error(const char* format, ...);
    void warn(const char* format, ...);
    void info(const char* format, ...);
    void debug(const char* format, ...);
    void trace(const char* format, ...);

private:
    char m_id[20];
    Sink& m_sink;
    Level m_threshold;

    void log(Level level, const char* format, va_list ap);
    void log_small(Level level, const char* format, va_list ap);
    void log_large(Level level, const char* format, va_list ap);
};

}
}
