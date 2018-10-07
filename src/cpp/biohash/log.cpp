#include <stdio.h>
#include <cstring>
#include <unistd.h>

#include "log.hpp"
#include "time.hpp"
#include "assert.hpp"

using namespace biohash;

const char* log::level_to_string(Level level)
{
    switch (level) {
        case Level::off:
            return "off";
        case Level::fatal:
            return "fatal";
        case Level::error:
            return "error";
        case Level::warn:
            return "warn";
        case Level::info:
            return "info";
        case Level::debug:
            return "debug";
        case Level::trace:
            return "trace";
    }
}

bool log::string_to_level(const char* str, Level& level)
{
    if (std::strcmp(str, "off") == 0) {
        level = Level::off;
        return true;
    }
    else if (std::strcmp(str, "fatal") == 0) {
        level = Level::fatal;
        return true;
    }
    else if (std::strcmp(str, "error") == 0) {
        level = Level::error;
        return true;
    }
    else if (std::strcmp(str, "warn") == 0) {
        level = Level::warn;
        return true;
    }
    else if (std::strcmp(str, "info") == 0) {
        level = Level::info;
        return true;
    }
    else if (std::strcmp(str, "debug") == 0) {
        level = Level::debug;
        return true;
    }
    else if (std::strcmp(str, "trace") == 0) {
        level = Level::trace;
        return true;
    }
    return false;
}

void log::NullSink::put(const char*, size_t)
{
}

void log::StderrSink::put(const char* data, size_t size)
{
    int fd = fileno(stderr);
    write(fd, data, size);
}

log::Logger::Logger(const std::string& id, Sink& sink, Level threshold):
    m_sink {sink},
    m_threshold {threshold}
{
    ASSERT(id.size() < 20);
    strcpy(m_id, id.c_str());
}

void log::Logger::fatal(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    log(Level::fatal, format, ap);
    va_end(ap);
}

void log::Logger::error(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    log(Level::error, format, ap);
    va_end(ap);
}

void log::Logger::warn(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    log(Level::warn, format, ap);
    va_end(ap);
}

void log::Logger::info(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    log(Level::info, format, ap);
    va_end(ap);
}

void log::Logger::debug(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    log(Level::debug, format, ap);
    va_end(ap);
}

void log::Logger::trace(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    log(Level::trace, format, ap);
    va_end(ap);
}

void log::Logger::log(Level level, const char* format, va_list ap)
{
    if (level > m_threshold)
        return;

    constexpr size_t limit = 512;
    char buf_1[limit];
    size_t pos;
    size_t size;

    size = snprintf(buf_1, 20, "[%s][", m_id);
    ASSERT(size < 20);
    pos = size;

    size = biohash::time::formatted_now(buf_1 + pos, 24);
    ASSERT(size < 24);
    pos += size;

    size = snprintf(buf_1 + pos, 11, "][%s]: ", level_to_string(level));
    ASSERT(size < 11);
    pos += size;

    va_list ap_2;
    va_copy(ap_2, ap);
    size = vsnprintf(buf_1 + pos, limit - pos, format, ap);
    if (size < limit - pos) {
        pos += size;
        buf_1[pos] = '\n';
        m_sink.put(buf_1, pos + 1);
    }
    else {
        // Heap allocate
        std::unique_ptr<char[]> buf_2 {new char[pos + size + 1]};
        memcpy(buf_2.get(), buf_1, pos);
        size_t size_2 = vsnprintf(buf_2.get() + pos, size + 1, format, ap_2);
        ASSERT(size == size_2);
        pos += size;
        buf_2[pos] = '\n';
        m_sink.put(buf_2.get(), pos + 1);
    }
    va_end(ap_2);
}
