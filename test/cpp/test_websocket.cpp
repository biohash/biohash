#include <iostream>

#include "util/test.hpp"

#include <biohash/websocket.hpp>
#include <biohash/base64.hpp>
#include <biohash/http.hpp>

using namespace biohash;
using namespace biohash::test;
using Message = http::Message;

TEST(websocket_1)
{
    char sec_websocket_key[24];
    websocket::make_sec_websocket_key(sec_websocket_key);
    CHECK_EQUAL(base64::decoded_size(sec_websocket_key, 24), 16);
}

TEST(websocket_2)
{
    const char sec_websocket_key[] = "dGhlIHNhbXBsZSBub25jZQ==";
    static_assert(sizeof(sec_websocket_key) == 25);
    CHECK(websocket::validate_sec_websocket_key(sec_websocket_key, 24));

    const char expected[] = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
    static_assert(sizeof(expected) == 29);
    char sec_websocket_accept[28];
    websocket::calculate_sec_websocket_accept(sec_websocket_key, sec_websocket_accept);
    CHECK(memcmp(sec_websocket_accept, expected, 28) == 0);
}

TEST(websocket_3)
{
    const char sec_websocket_key[] = "abcd";
    CHECK(!websocket::validate_sec_websocket_key(sec_websocket_key, 4));
}

TEST(websocket_4)
{
    const char sec_websocket_key[] = "dGhlIHNhbXBsZSBub25jZQ11";
    static_assert(sizeof(sec_websocket_key) == 25);
    CHECK(!websocket::validate_sec_websocket_key(sec_websocket_key, 24));
}

TEST(websocket_5)
{
    char request[256];
    const char* request_target = "/home";
    size_t osize = http::write_request_line(request, 256, http::Method::GET, request_target);
    CHECK_EQUAL(osize, 20);
    
    const char protocol[] = "chat";
    osize = websocket::write_client_handshake_headers(request + 20, 236, protocol);
    CHECK_EQUAL(osize, 143);

    osize = http::write_header_end(request + 163, 93);
    CHECK_EQUAL(osize, 2);
    
    Message parser {Message::Kind::Request, request, 256};

    CHECK(parser.complete);
    CHECK(parser.valid);
    CHECK_EQUAL(parser.message_size, 165);
    CHECK(parser.kind == Message::Kind::Request);
    CHECK_EQUAL(parser.content_length, 0);
    CHECK(parser.body == request + 165);
    CHECK(parser.method == http::Method::GET);
    CHECK(parser.request_target == "/home");
    CHECK(parser.header_upgrade == "websocket");
    CHECK(parser.header_connection == "Upgrade");
    CHECK(parser.header_sec_websocket_version == "13");
    CHECK(parser.header_sec_websocket_protocol == "chat");
    CHECK(parser.header_sec_websocket_key.size() == 24);

    CHECK(websocket::validate_client_handshake(parser));
    const char sec_websocket_key[] = "dGhlIHNhbXBsZSBub25jZQ==";
    CHECK(!websocket::validate_server_handshake(parser, sec_websocket_key));
}

TEST(websocket_6)
{
    char response[256];
    const char sec_websocket_key[] = "dGhlIHNhbXBsZSBub25jZQ==";
    const char protocol[] = "chat";
    size_t osize =
        websocket::write_server_handshake_status_and_headers(response, 256, sec_websocket_key, protocol);
    CHECK_EQUAL(osize, 157);
    osize = http::write_header_end(response + 157, 99);
    CHECK_EQUAL(osize, 2);
    
    Message parser {Message::Kind::Response, response, 256};

    CHECK(parser.complete);
    CHECK(parser.valid);
    CHECK_EQUAL(parser.message_size, 159);
    CHECK(parser.kind == Message::Kind::Response);
    CHECK_EQUAL(parser.content_length, 0);
    CHECK(parser.body == response + 159);
    CHECK(parser.status_code == 101);
    CHECK(parser.reason_phrase == "Switching Protocols");
    CHECK(parser.header_upgrade == "websocket");
    CHECK(parser.header_connection == "Upgrade");
    CHECK(parser.header_sec_websocket_protocol == "chat");
    const char accept[] = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
    CHECK(parser.header_sec_websocket_accept == accept);

    CHECK(websocket::validate_server_handshake(parser, sec_websocket_key));
    CHECK(!websocket::validate_client_handshake(parser));
}

TEST(websocket_7)
{
    const char request[] =
        "GET /chat HTTP/1.1\r\n"
        "Host: server.example.com\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Nonsense\r\n" // Changed Upgrade to Nonsense.
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Origin: http://example.com\r\n"
        "Sec-WebSocket-Protocol: chat, superchat\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message parser {Message::Kind::Request, request, size};
    CHECK(parser.complete);
    CHECK(parser.valid);

    CHECK(!websocket::validate_client_handshake(parser));
}

TEST(websocket_8)
{
    const char response[] =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: S3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n" // Changed 's' to 'S'.
        "Sec-WebSocket-Protocol: chat\r\n"
        "\r\n";
    size_t size = sizeof(response) - 1;

    Message parser {Message::Kind::Response, response, size};
    CHECK(parser.complete);
    CHECK(parser.valid);

    const char sec_websocket_key[] = "dGhlIHNhbXBsZSBub25jZQ==";
    CHECK(!websocket::validate_server_handshake(parser, sec_websocket_key));
}
