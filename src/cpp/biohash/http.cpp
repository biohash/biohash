#include <string.h>
#include <strings.h>
#include <limits.h>
#include <inttypes.h>

#include "http.hpp"
#include "assert.hpp"

using namespace biohash;

const char* http::method_str(Method method)
{
    switch (method) {
        case Method::GET:
            return "GET";
        case Method::HEAD:
            return "HEAD";
        case Method::POST:
            return "POST";
        case Method::PUT:
            return "PUT";
        case Method::DELETE:
            return "DELETE";
        case Method::CONNECT:
            return "CONNECT";
        case Method::OPTIONS:
            return "OPTIONS";
        case Method::TRACE:
            return "TRACE";
    }
}

const char* http::reason_phrase(int status_code)
{
    switch(status_code) {
        case 101:
            return "Switching Protocol";
        case 200:
            return "OK";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 404:
            return "Not Found";
        case 503:
            return "Service Unavailable";
        default:
            ASSERT(false);
            return nullptr;
    }
}

size_t http::write_request_line(char* buf, size_t size, Method method,
                                const char* request_target)
{
   const char* meth = method_str(method);

   size_t rc = static_cast<size_t>(snprintf(buf, size, "%s %s HTTP/1.1\r",
                                            meth, request_target));
   if (rc < size)
       buf[rc] = '\n';
   return rc + 1;
}

size_t http::write_status_line(char* buf, size_t size, int status_code)
{
    int status = static_cast<int>(status_code);
    const char* reason = reason_phrase(status_code);
    size_t rc = static_cast<size_t>(snprintf(buf, size, "HTTP/1.1 %i %s\r",
                                             status, reason));

    if (rc < size)
        buf[rc] = '\n';
    return rc + 1;
}

size_t http::write_header(char* buf, size_t size, const char* name, const char* value)
{
    size_t rc = static_cast<size_t>(snprintf(buf, size, "%s: %s\r", name, value));
    if (rc < size)
        buf[rc] = '\n';
    return rc + 1;
}

http::MessageParser::MessageParser(Kind kind, const char* buf, size_t buf_size):
    kind {kind},
    buf {buf},
    buf_size {buf_size},
    cur {buf},
    end {buf + buf_size}
{
    if (!parse_start_line())
        return;

    if (!parse_headers())
        return;

   if (!parse_body())
        return;

    complete = true;
    return;
}

bool http::MessageParser::parse_start_line()
{
        return (kind == Kind::Request) ? parse_request_line() : parse_status_line();
}

bool http::MessageParser::parse_request_line()
{
    while (cur < end && (cur - buf) < 8 && *cur != ' ')
        ++cur;
    if (cur == end)
        return false;
    if (*cur != ' ') {
        valid = false;
        return false;
    }

    if (cur == buf + 3 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T')
        method = Method::GET;
    else if (cur == buf + 4 && buf[0] == 'H' && buf[1] == 'E' && buf[2] == 'A' && buf[3] == 'D')
        method = Method::HEAD;
    else if (cur == buf + 4 && buf[0] == 'P' && buf[1] == 'O' && buf[2] == 'S' && buf[3] == 'T')
        method = Method::POST;
    else if (cur == buf + 3 && buf[0] == 'P' && buf[1] == 'U' && buf[2] == 'T')
        method = Method::PUT;
    else if (cur == buf + 6 && buf[0] == 'D' && buf[1] == 'E' && buf[2] == 'L' && buf[3] == 'E'
             && buf[4] == 'T' && buf[5] == 'E')
        method = Method::DELETE;
    else if (cur == buf + 7 && buf[0] == 'C' && buf[1] == 'O' && buf[2] == 'N' && buf[3] == 'N'
             && buf[4] == 'E' && buf[5] == 'C' && buf[6] == 'T')
        method = Method::CONNECT;
    else if (cur == buf + 7 && buf[0] == 'O' && buf[1] == 'P' && buf[2] == 'T' && buf[3] == 'I'
             && buf[4] == 'O' && buf[5] == 'N' && buf[6] == 'S')
        method = Method::OPTIONS;
    else if (cur == buf + 5 && buf[0] == 'T' && buf[1] == 'R' && buf[2] == 'A' && buf[3] == 'C'
             && buf[4] == 'E')
        method = Method::TRACE;
    else {
        valid = false;
        return false;
    }

    ++cur;
    while (cur != end && *cur == ' ')
        ++cur;
    if (cur == end)
        return false;
    const char* request_target_data = cur;
    while (cur < end && *cur != ' ')
        ++cur;
    if (cur == end)
        return false;
    size_t request_target_size = cur - request_target_data;
    request_target = std::string_view {request_target_data, request_target_size};

    while (cur != end && *cur == ' ')
        ++cur;
    if (cur == end)
        return false;

    // Expect "HTTP/1.1\r\n"
    if (cur + 10 > end)
        return false;
    if (memcmp(cur, "HTTP/1.1\r\n", 10) != 0) {
        valid = false;
        return false;
    }
    cur += 10;
    return true;
}

bool http::MessageParser::parse_status_line()
{
    if (cur + 9 > end)
        return false;
    if (memcmp(cur, "HTTP/1.1 ", 9) != 0) {
        valid = false;
        return false;
    }
    cur += 9;
    while (cur < end && *cur == ' ')
        ++cur;
    if (cur == end)
        return false;
    if (*cur < '1' || *cur > '9') {
        valid = false;
        return false;
    }
    status_code = *cur - '0';
    for (int i = 0; i < 2; ++i) {
        ++cur;
        if (cur == end)
            return false;
        if (*cur < '0' || *cur > '9') {
            valid = false;
            return false;
        }
        status_code = 10 * status_code + (*cur - '0');
    }
    ++cur;
    if (cur == end)
        return false;
    if (*cur != ' ') {
        valid = false;
        return false;
    }
    ++cur;
    while (cur < end && *cur == ' ')
        ++cur;
    if (cur == end)
        return false;
    const char* reason_phrase_data = cur;
    while (cur < end && *cur != '\r')
        ++cur;
    if (cur == end)
        return false;
    reason_phrase = std::string_view {reason_phrase_data,
        static_cast<size_t>(cur - reason_phrase_data)};
    ++cur;
    if (cur == end)
        return false;
    if (*cur != '\n') {
        valid = false;
        return false;
    }
    ++cur;
    return true;
}

bool http::MessageParser::parse_headers()
{
    while (cur != end && *cur != '\r') {
        bool rc = parse_header();
        if (!rc)
            return false;
    }
    if (cur == end)
        return false;
    ASSERT(*cur == '\r');
    ++cur;
    if (cur == end)
        return false;
    if (*cur != '\n') {
        valid = false;
        return false;
    }
    ++cur;
    return true;
}

bool http::MessageParser::parse_header()
{
    while (*cur == ' ')
        ++cur;
    if (cur == end)
        return false;

    const char* name = cur;
    if (*name == ':') {
        valid = false;
        return false;
    }

    while(cur != end && *cur != ':' && *cur != ' ')
        ++cur;
    if (cur == end)
        return false;

    size_t name_size = cur - name;
    ASSERT(name_size > 0);

    while(cur != end && *cur != ':')
        ++cur;
    if (cur == end)
        return false;

    ++cur;
    while (cur != end && *cur == ' ')
        ++cur;
    if (cur == end)
        return false;

    const char* value = cur;
    while (cur != end && *cur != '\r')
        ++cur;
    if (cur == end)
        return false;
    size_t value_size = cur - value;
    ++cur;
    if (cur == end)
        return false;
    if (*cur != '\n') {
        valid = false;
        return false;
    }
    ++cur;

    while (value_size > 0 && *(value + (value_size - 1)) == ' ')
        --value_size;

    int rc =  interpret_header(name, name_size, value, value_size);
    if (!rc)
        return false;

    return true;
}

#include <string>
bool http::MessageParser::interpret_header(const char* name, size_t name_size,
                                           const char* value, size_t value_size)
{
    std::string name1(name, name_size);
    std::string value1(value, value_size);
    //printf("Name: %s\n", name1.c_str());
    //printf("Value: %s\n", value1.c_str());

    const char* content_length_value = "Content-Length";
    const char* authorization_value = "Authorization";
    const char* user_agent_value = "User-Agent";
    const char* host_value = "Host";
    const char* upgrade_value = "Upgrade";
    const char* connection_value = "Connection";
    const char* origin_value = "Origin";
    const char* sec_websocket_protocol_value = "Sec-WebSocket-Protocol";
    const char* sec_websocket_version_value = "Sec-WebSocket-Version";
    const char* sec_websocket_accept_value = "Sec-WebSocket-Accept";
    const char* transfer_encoding_value = "Transfer-Encoding";

    if (name_size == 14 && strncasecmp(content_length_value, name, 14) == 0) {
        ASSERT('\0' == 0);
        ASSERT(sizeof(size_t) >= 8);
        char buf[17] = { };
        if (value_size > 16)
            return false;
        memcpy(buf, value, value_size);
        ASSERT(buf[value_size] == '\0');
        char *endptr = nullptr;
        intmax_t cl = strtoimax(buf, &endptr, 10);
        if (!endptr || *endptr != '\0' || cl < 0) {
            valid = false;
            return false;
        }
        content_length = static_cast<size_t>(cl); // overflow impossible
        ASSERT(static_cast<intmax_t>(content_length) == cl);
    }
    else if (name_size == 13 && strncasecmp(authorization_value, name, 13) == 0)
        header_authorization = std::string_view {value, value_size};
    else if (name_size == 10 && strncasecmp(user_agent_value, name, 10) == 0)
        header_user_agent = std::string_view {value, value_size};
    else if (name_size == 4 && strncasecmp(host_value, name, 4) == 0)
        header_host = std::string_view {value, value_size};
    else if (name_size == 7 && strncasecmp(upgrade_value, name, 7) == 0)
        header_upgrade = std::string_view {value, value_size};
    else if (name_size == 10 && strncasecmp(connection_value, name, 10) == 0)
        header_connection = std::string_view {value, value_size};
    else if (name_size == 6 && strncasecmp(origin_value, name, 6) == 0)
        header_origin = std::string_view {value, value_size};
    else if (name_size == 22 && strncasecmp(sec_websocket_protocol_value, name, 22) == 0)
        header_sec_websocket_protocol = std::string_view {value, value_size};
    else if (name_size == 21 && strncasecmp(sec_websocket_version_value, name, 21) == 0)
        header_sec_websocket_version = std::string_view {value, value_size};
    else if (name_size == 20 && strncasecmp(sec_websocket_accept_value, name, 20) == 0)
        header_sec_websocket_accept = std::string_view {value, value_size};
    else if (name_size == 17 && strncasecmp(transfer_encoding_value, name, 17) == 0) {
        // Don't handle at the moment.
        valid = false;
        return false;
    }

    return true;
}

bool http::MessageParser::parse_body()
{
    body = cur;
    message_size = (cur - buf) + content_length;
    if (message_size > buf_size)
        return false;

    ASSERT(valid);
    return true;
}
