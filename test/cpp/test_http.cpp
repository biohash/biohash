#include <iostream>

#include "util/test.hpp"

#include <biohash/http.hpp>

using namespace biohash;
using namespace biohash::test;
using Message = http::Message;

TEST(http_write_request_line)
{
    char buf[32];
    const char* request_target = "/home";
    size_t osize = http::write_request_line(buf, 32, http::Method::GET, request_target);
    CHECK_EQUAL(osize, 20);
    CHECK_MEMCMP(buf, "GET /home HTTP/1.1\r\n", 20);

    osize = http::write_request_line(buf, 15, http::Method::GET, request_target);
    CHECK_EQUAL(osize, 20);
}

TEST(http_write_status_line)
{
    char buf[32];

    size_t osize = http::write_status_line(buf, 32, 200);
    CHECK_EQUAL(osize, 17);
    CHECK_MEMCMP(buf, "HTTP/1.1 200 OK\r\n", 17);

    osize = http::write_status_line(buf, 10, 200);
    CHECK_EQUAL(osize, 17);

    osize = http::write_status_line(buf, 32, 404);
    CHECK_EQUAL(osize, 24);
    CHECK_MEMCMP(buf, "HTTP/1.1 404 Not Found\r\n", 24);
}

TEST(http_write_header)
{
    char buf[32];

    size_t osize = http::write_header(buf, 32, "Host", "www.biohash.org");
    CHECK_EQUAL(osize, 23);
    CHECK_MEMCMP(buf, "Host: www.biohash.org\r\n", 23);

    osize = http::write_header(buf, 22, "Host", "www.biohash.org");
    CHECK_EQUAL(osize, 23);

    buf[23] = 0;
    osize = http::write_header(buf, 23, "Host", "www.biohash.org");
    CHECK_EQUAL(osize, 23);
    CHECK_EQUAL(buf[23], 0);
}

TEST(http_msg_0)
{
    const char request[] =
        "GET /home HTTP/1.1\r\n"
        "Host: www.biohash.org\r\n"
        "User-Agent: CERN-LineMode/2.15 libwww/2.17b3\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK(msg.header_user_agent == "CERN-LineMode/2.15 libwww/2.17b3");
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Request);
    CHECK_EQUAL(msg.content_length, 0);
    CHECK(msg.body == request + size);
    CHECK(msg.method == http::Method::GET);
    CHECK(msg.request_target == "/home");
}

TEST(http_msg_1)
{
    const char request[] =
        "POST /home/article HTTP/1.1\r\n"
        "Content-Length: 3\r\n"
        "\r\n"
        "abc";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Request);
    CHECK_EQUAL(msg.content_length, 3);
    CHECK(msg.body == request + size - 3);
    CHECK(msg.method == http::Method::POST);
    CHECK(msg.request_target == "/home/article");
}

TEST(http_msg_2)
{
    const char request[] =
        "GET /chat HTTP/1.1\r\n"
        "Host: server.example.com\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Origin: http://example.com\r\n"
        "Sec-WebSocket-Protocol: chat, superchat\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Request);
    CHECK_EQUAL(msg.content_length, 0);
    CHECK(msg.method == http::Method::GET);
    CHECK(msg.request_target == "/chat");
    CHECK(msg.header_host == "server.example.com");
    CHECK(msg.header_upgrade == "websocket");
    CHECK(msg.header_connection == "Upgrade");
    CHECK(msg.header_origin == "http://example.com");
    CHECK(msg.header_sec_websocket_protocol == "chat, superchat");
    CHECK(msg.header_sec_websocket_version == "13");
}

TEST(http_msg_3)
{
    const char response[] =
        "HTTP/1.1 200 OK\r\n"
        "\r\n";
    size_t size = sizeof(response) - 1;

    Message msg {Message::Kind::Response, response, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Response);
    CHECK_EQUAL(msg.content_length, 0);
    CHECK(msg.status_code == 200);
    CHECK(msg.reason_phrase == "OK");
}

TEST(http_msg_4)
{
    const char response[] =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n"
        "Sec-WebSocket-Protocol: chat\r\n"
        "\r\n";
    size_t size = sizeof(response) - 1;

    Message msg {Message::Kind::Response, response, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Response);
    CHECK_EQUAL(msg.content_length, 0);
    CHECK(msg.status_code == 101);
    CHECK(msg.reason_phrase == "Switching Protocols");
    CHECK(msg.header_upgrade == "websocket");
    CHECK(msg.header_connection == "Upgrade");
    CHECK(msg.header_sec_websocket_accept == "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
    CHECK(msg.header_sec_websocket_protocol == "chat");
}

TEST(http_msg_5)
{
    const char response[] =
        "HTTP/1.1  101   Switching Protocols\r\n"
        "  Upgrade  :    websocket\r\n"
        "Connection: Upgrade \r\n"
        "\r\n";
    size_t size = sizeof(response) - 1;

    Message msg {Message::Kind::Response, response, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Response);
    CHECK_EQUAL(msg.content_length, 0);
    CHECK(msg.status_code == 101);
    CHECK(msg.reason_phrase == "Switching Protocols");
    CHECK(msg.header_upgrade == "websocket");
    CHECK(msg.header_connection == "Upgrade");
}

TEST(http_msg_6)
{
    const char request[] =
        "PUT         /home/article    HTTP/1.1\r\n"
        "  Content-Length    :     3     \r\n"
        "\r\n"
        "abc";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Request);
    CHECK_EQUAL(msg.content_length, 3);
    CHECK(msg.body == request + size - 3);
    CHECK(msg.method == http::Method::PUT);
    CHECK(msg.request_target == "/home/article");
}

TEST(http_msg_7)
{
    const char request[] =
        "DELETE /home/article%45 HTTP/1.1\r\n"
        "Content-Length:\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Request);
    CHECK_EQUAL(msg.content_length, 0);
    CHECK(msg.body == request + size);
    CHECK(msg.method == http::Method::DELETE);
    CHECK(msg.request_target == "/home/article%45");
}

TEST(http_msg_8)
{
    const char request[] =
        "TRACE / HTTP/1.1\r\n"
        "User-Agent:\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Request);
    CHECK_EQUAL(msg.content_length, 0);
    CHECK(msg.body == request + size);
    CHECK(msg.method == http::Method::TRACE);
    CHECK(msg.request_target == "/");
    CHECK(msg.header_user_agent.empty());
}

TEST(http_msg_9)
{
    const char request[] =
        "HEAD // HTTP/1.1\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size);
    CHECK(msg.kind == Message::Kind::Request);
    CHECK_EQUAL(msg.content_length, 0);
    CHECK(msg.body == request + size);
    CHECK(msg.method == http::Method::HEAD);
    CHECK(msg.request_target == "//");
    CHECK(!msg.header_user_agent.data());
}

TEST(http_msg_10)
{
    const char request[] =
        "CONNECT /a HTTP/1.1\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK(msg.method == http::Method::CONNECT);
}

TEST(http_msg_11)
{
    const char request[] =
        "OPTIONS /,. HTTP/1.1\r\n"
        "Authorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK(msg.method == http::Method::OPTIONS);
    CHECK(msg.request_target == "/,.");
    CHECK(msg.header_authorization == "Basic YWxhZGRpbjpvcGVuc2VzYW1l");
}

TEST(http_msg_12)
{
    const char request[] =
        "NONE /home HTTP/1.1\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(!msg.complete);
    CHECK(!msg.valid);
}

TEST(http_msg_13)
{
    const char request[] =
        "GET /home HTTP/1.0\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(!msg.complete);
    CHECK(!msg.valid);
}

TEST(http_msg_14)
{
    const char request[] =
        "GET /home HTTP/1.1\r\n"
        "Home: www.origin.com\r \n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(!msg.complete);
    CHECK(!msg.valid);
}

TEST(http_msg_15)
{
    const char request[] =
        "GET / HTTP/1.1\r\n"
        "authorization: Something \r\n"
        "USER-AGENT: agent\r\n"
        "\r\n";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK(msg.method == http::Method::GET);
    CHECK(msg.request_target == "/");
    CHECK(msg.header_authorization == "Something");
    CHECK(msg.header_user_agent == "agent");
}

TEST(http_msg_16)
{
    const char request[] =
        "POST /home/article HTTP/1.1\r\n"
        "Content-Length: 3\r\n"
        "\r\n"
        "abcdefg";
    size_t size = sizeof(request) - 1;

    Message msg {Message::Kind::Request, request, size};

    CHECK(msg.complete);
    CHECK(msg.valid);
    CHECK_EQUAL(msg.message_size, size - 4);
    CHECK(msg.kind == Message::Kind::Request);
    CHECK_EQUAL(msg.content_length, 3);
    CHECK(msg.body == request + size - 7);
    CHECK(msg.method == http::Method::POST);
    CHECK(msg.request_target == "/home/article");
}
