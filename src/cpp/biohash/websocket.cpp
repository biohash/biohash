#include <string.h>
#include <strings.h>
#include <limits.h>
#include <inttypes.h>
#include <stdlib.h>

#include <bearssl_hash.h>

#include "websocket.hpp"
#include "assert.hpp"
#include "base64.hpp"


using namespace biohash;

void websocket::make_sec_websocket_key(char* sec_websocket_key)
{
    char rand[16];
    arc4random_buf(rand, 16);
    size_t encoded_size = base64::encode(rand, 16, sec_websocket_key);
    ASSERT(encoded_size == 24);
}

bool websocket::validate_sec_websocket_key(const char* sec_websocket_key, size_t size)
{
    if (size != 24)
        return false;
    char decoded_data[18];
    size_t decoded_size;
    bool rc = base64::decode(sec_websocket_key, 24, decoded_data, decoded_size);
    return rc && decoded_size == 16;
}

void websocket::calculate_sec_websocket_accept(const char* sec_websocket_key, char* sec_websocket_accept)
{
    const char magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    static_assert(sizeof(magic) == 36 + 1);
    char concat[60];
    memcpy(concat, sec_websocket_key, 24);
    memcpy(concat + 24, magic, 36);
    
    char sha1[20];
    br_sha1_context ctx;
    br_sha1_init(&ctx);
    br_sha1_update(&ctx, concat, 60);
    br_sha1_out(&ctx, sha1);

    size_t sec_websocket_accept_size = base64::encode(sha1, 20, sec_websocket_accept);
    ASSERT(sec_websocket_accept_size == 28);
}

size_t websocket::write_client_handshake_headers(char* buf, size_t size, const char* protocol)
{
    size_t size_total = 0;
    size_t size_header;

    size_header = http::write_header(buf, size, "Upgrade", "websocket");
    size_total += size_header;
    if (size_total > size)
        return size_total;

    size_header = http::write_header(buf + size_total, size, "Connection", "Upgrade");
    size_total += size_header;
    if (size_total > size)
        return size_total;

    char sec_websocket_key[25];
    make_sec_websocket_key(sec_websocket_key);
    sec_websocket_key[24] = '\0';

    size_header = http::write_header(buf + size_total, size, "Sec-WebSocket-Key", sec_websocket_key);
    size_total += size_header;
    if (size_total > size)
        return size_total;

    size_header = http::write_header(buf + size_total, size, "Sec-WebSocket-Protocol", protocol);
    size_total += size_header;
    if (size_total > size)
        return size_total;

    size_header = http::write_header(buf + size_total, size, "Sec-WebSocket-Version", "13");
    size_total += size_header;
    return size_total;
}

size_t websocket::write_server_handshake_status_and_headers(char* buf, size_t size,
                                                            const char* sec_websocket_key,
                                                            const char* protocol)
{
    size_t size_total = 0;
    size_t size_line;

    size_line = http::write_status_line(buf, size, 101);
    size_total += size_line;
    if (size_total > size)
        return size_total;

    size_line = http::write_header(buf + size_total, size, "Upgrade", "websocket");
    size_total += size_line;
    if (size_total > size)
        return size_total;

    size_line = http::write_header(buf + size_total, size, "Connection", "Upgrade");
    size_total += size_line;
    if (size_total > size)
        return size_total;

    char sec_websocket_accept[29];
    calculate_sec_websocket_accept(sec_websocket_key, sec_websocket_accept);
    sec_websocket_accept[28] = '\0';

    size_line = http::write_header(buf + size_total, size, "Sec-WebSocket-Accept", sec_websocket_accept);
    size_total += size_line;
    if (size_total > size)
        return size_total;

    size_line = http::write_header(buf + size_total, size, "Sec-WebSocket-Protocol", protocol);
    size_total += size_line;
    return size_total;
}

bool websocket::validate_client_handshake(const http::Message& request)
{
    if (request.kind != http::Message::Kind::Request)
        return false;

    if (request.header_upgrade.size() != 9 || strncasecmp(request.header_upgrade.data(), "websocket", 9) != 0)
        return false;

    if (request.header_connection.size() != 7 || strncasecmp(request.header_connection.data(), "Upgrade", 7) != 0)
        return false;

    if (request.header_sec_websocket_version != "13")
        return false;

    if (!validate_sec_websocket_key(request.header_sec_websocket_key.data(),
                                    request.header_sec_websocket_key.size()))
        return false;

    return true;
}

bool websocket::validate_server_handshake(const http::Message& response,
                                          const char* sec_websocket_key)
{
    if (response.kind != http::Message::Kind::Response)
        return false;

    if (response.header_upgrade.size() != 9 || strncasecmp(response.header_upgrade.data(), "websocket", 9) != 0)
        return false;

    if (response.header_connection.size() != 7 || strncasecmp(response.header_connection.data(), "Upgrade", 7) != 0)
        return false;

    char sec_websocket_accept[29];
    calculate_sec_websocket_accept(sec_websocket_key, sec_websocket_accept);
    sec_websocket_accept[28] = '\0';

    if (response.header_sec_websocket_accept != sec_websocket_accept)
        return false;

    return true;
}
