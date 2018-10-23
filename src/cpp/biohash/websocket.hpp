#pragma once

#include <stddef.h>
#include <string_view>

#include "http.hpp"

namespace biohash {
namespace websocket {

// sec_websocket_key has size 24 and is the base64 encoding of a random 16 byte
// value.
void make_sec_websocket_key(char* sec_websocket_key);

// The sec_websocket_key must be the Base64 encoding of a 16 byte (random)
// value.
bool validate_sec_websocket_key(const char* sec_websocket_key, size_t size);

// sec_websocket_key must be valid and hence have size 24. The result will be
// placed in sec_websocket_accept and have size 28.
void calculate_sec_websocket_accept(const char* sec_websocket_key, char*
                                    sec_websocket_accept);

// write_client_handshake_headers() add HTTP headers to the buffer 'buf' of
// size 'size' and returns the size of the added headers. if 'size' is less
// than the return value, the added headers will only be partially added and
// the result should not be trusted. The argument 'protocol' must be a null
// terminated string. The size of the headers is 139 + the size of 'protocol'.
//
// The added headers are:
// Upgrade: websocket
// Connection: Upgrade
// Sec-WebSocket-Key: 'A generated key'
// Sec-WebSocket-Protocol: 'protocol'
// Sec-WebSocket-Version: 13
size_t write_client_handshake_headers(char* buf, size_t size, const char* protocol);

// write_server_handshake_status_and_headers() writes a HTTP response status line and 
// the WebSocket headers into the buffer. The caller can add more headers and must end
// the response. The return value is the added size. If the return value is less than
// 'size' the message has only been partially added. The size of the added message is
// ??? + the size of 'protocol'.
//
// The added status line and headers are:
// HTTP/1.1 101 Switching Protocols
// Upgrade: websocket
// Connection: Upgrade
// Sec-WebSocket-Accept: 'A calculated accept'
// Sec-WebSocket-Protocol: 'protocol'
size_t write_server_handshake_status_and_headers(char* buf, size_t size,
                                                 const char* sec_websocket_key,
                                                 const char* protocol);

bool validate_client_handshake(const http::Message& request);
bool validate_server_handshake(const http::Message& response, const char* sec_websocket_key);




}
}
