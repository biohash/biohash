#pragma once

#include <stddef.h>
#include <string_view>

namespace biohash {
namespace http {

// HTTP/1.1

enum class Method {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE
};

const char* method_str(Method method);

const char* reason_phrase(int status_code);

// Functions to make HTTP messages
//
// The functions take a buffer 'buf' of size 'size' and inserts the HTTP line
// or headers.  The return value is the size of the line. If 'size'
// is less than the return value, the line has not been written.

// request-line = method SP request-target SP HTTP/1.1 CRLF
size_t write_request_line(char* buf, size_t size, Method method, const char* request_target);

// status-line = HTTP/1.1 SP status-code SP reason-phrase CRLF
size_t write_status_line(char* buf, size_t size, int status_code);

// name: SP value CRLF
size_t write_header(char* buf, size_t size, const char* name, const char* value);

// A MessageParser object is constructed with a const buffer as argument.
// Owenership is not transferred to MessageParser. MessageParser parses the
// buffer and determines whether a complete HTTP message is contained in the
// buffer, and reports various properties of the message in its public member
// variables.  The properties of the message should only be used when complete
// is true. If valid is false, after complete is true, the message did not
// conform to the specification.
class MessageParser {
public:

    enum class Kind {
        Request,
        Response
    };

    MessageParser(Kind kind, const char* buf, size_t buf_size);

    Kind kind;
    const char* const buf;
    size_t buf_size;

    const char* cur;
    const char* const end;

    // Common to all messages.
    bool complete = false;
    bool valid = true;
    size_t message_size = 0;
    const char* body;
    uint_least64_t content_length = 0;

    // Request specific
    Method method;
    std::string_view request_target;

    // Response specific
    int status_code;
    std::string_view reason_phrase;

    // Headers
    std::string_view header_authorization;
    std::string_view header_user_agent;
    std::string_view header_host;
    std::string_view header_upgrade;
    std::string_view header_connection;
    std::string_view header_origin;
    std::string_view header_sec_websocket_protocol;
    std::string_view header_sec_websocket_version;
    std::string_view header_sec_websocket_accept;

private:

    bool parse_start_line();
    bool parse_request_line();
    bool parse_status_line();

    bool parse_headers();
    bool parse_header();
    bool interpret_header(const char* name, size_t name_size,
                          const char* value, size_t value_size);
    bool parse_body();
};


}
}
