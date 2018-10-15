#include <math.h>

#include <biohash/json.hpp>

#include "util/test.hpp"

using namespace biohash;
using namespace biohash::test;
using namespace biohash::json;

TEST(json_token_number)
{
    long double number = 123.45;
    Token token;
    token.type = Token::Type::Number;
    token.payload.number = number;
    Token token_1 = token;
    CHECK(token_1.type == Token::Type::Number);
    CHECK_EQUAL(token_1.payload.number, number);
}

TEST(json_token_string)
{
    const char data[] = "Hello";
    size_t size = sizeof(data);
    Token token;
    token.type = Token::Type::String;
    token.payload.string.data = data;
    token.payload.string.size = size;
    Token token_1 = token;
    CHECK(token_1.type == Token::Type::String);
    CHECK(token_1.payload.string.data == data);
    CHECK(token_1.payload.string.size == size);
}

TEST(json_tokenizer_whitespace)
{
    const char data[] = "\t\r \n  \r \t \n";
    size_t size = sizeof(data) - 1;
    Tokenizer tokenizer {data, size};
    CHECK(tokenizer.next().type == Token::Type::End);
}

TEST(json_tokenizer_null)
{
    const char data[] = "null";
    size_t size = sizeof(data) - 1;
    Tokenizer tokenizer {data, size};
    CHECK(tokenizer.next().type == Token::Type::Null);
    CHECK(tokenizer.next().type == Token::Type::End);
}

TEST(json_tokenizer_true)
{
    const char data[] = "true";
    size_t size = sizeof(data) - 1;
    Tokenizer tokenizer {data, size};
    CHECK(tokenizer.next().type == Token::Type::True);
    CHECK(tokenizer.next().type == Token::Type::End);
}

TEST(json_tokenizer_false)
{
    const char data[] = "false";
    size_t size = sizeof(data) - 1;
    Tokenizer tokenizer {data, size};
    CHECK(tokenizer.next().type == Token::Type::False);
    CHECK(tokenizer.next().type == Token::Type::End);
}

TEST(json_tokenizer_number)
{
    const char data[] = "123.45";
    size_t size = sizeof(data) - 1;
    Tokenizer tokenizer {data, size};
    Token token = tokenizer.next();
    CHECK(token.type == Token::Type::Number);
    CHECK(fabs(token.payload.number - 123.45) < 1e-10);
    CHECK(tokenizer.next().type == Token::Type::End);
}

TEST(json_tokenizer_string)
{
    const char data[] = "\"Biohash\"";
    size_t size = sizeof(data) - 1;
    Tokenizer tokenizer {data, size};
    Token token = tokenizer.next();
    CHECK(token.type == Token::Type::String);
    CHECK(token.payload.string.data == data + 1);
    CHECK(token.payload.string.size == size - 2);
    CHECK(tokenizer.next().type == Token::Type::End);
}

namespace {

struct JsonNumbers {
    const char* json;
    long double value;
};

}

TEST(json_tokenizer_numbers)
{
    const JsonNumbers json_numbers[] = {
        {"0", 0},
        {"7", 7},
        {"-1", -1},
        {"1.78", 1.78},
        {"-2.45", -2.45},
        {"-0.19", -0.19},
        {"2e-5", 2e-5},
        {"-3.4e+6", -3.4e6}
    };

    size_t n = sizeof(json_numbers) / sizeof(json_numbers[0]);

    for (size_t i = 0; i < n; ++i) {
        const char* data = json_numbers[i].json;
        const size_t size = strlen(json_numbers[i].json);
        long double value = json_numbers[i].value;
        Tokenizer tokenizer {data, size};
        Token token = tokenizer.next();
        CHECK(token.type == Token::Type::Number);
        CHECK(fabs(token.payload.number - value) < 1e-10);
        CHECK(tokenizer.next().type == Token::Type::End);
    }
}

namespace {

struct JsonStrings {
    const char* json;
    const char* value;
};

}

TEST(json_tokenizer_strings)
{
    const JsonStrings json_strings[] = {
        {"\"\"", ""},
        {"\"7\"", "7"},
        {"\"\\t\"", "\\t"},
        {"\"abc \\t \\r .,\\/'\"", "abc \\t \\r .,\\/'"},
        {"\"\\\"\"", "\\\""},
        {"\"\\u1234\"", "\\u1234"},
    };

    size_t n = sizeof(json_strings) / sizeof(json_strings[0]);

    for (size_t i = 0; i < n; ++i) {
        const char* data = json_strings[i].json;
        const size_t size = strlen(json_strings[i].json);
        const char* value = json_strings[i].value;
        size_t value_size = strlen(value);
        Tokenizer tokenizer {data, size};
        Token token = tokenizer.next();
        CHECK(token.type == Token::Type::String);
        CHECK_EQUAL(token.payload.string.size, value_size);
        CHECK(memcmp(token.payload.string.data, value, value_size) == 0);
        CHECK(tokenizer.next().type == Token::Type::End);
    }
}

TEST(json_tokenizer_multiple)
{
    const char data[] = "null \t truefalse \r { \n ][}\"str\"12.3-12.09 [";
    const size_t size = sizeof(data) - 1;
    Tokenizer tokenizer {data, size};
    Token token = tokenizer.next();
    CHECK(token.type == Token::Type::Null);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::True);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::False);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::ObjectBegin);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::ArrayEnd);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::ArrayBegin);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::ObjectEnd);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::String);
    CHECK_EQUAL(token.payload.string.size, strlen("str"));
    CHECK(memcmp(token.payload.string.data, "str", 3) == 0);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::Number);
    CHECK(fabs(token.payload.number - 12.3) < 1e-10);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::Number);
    CHECK(fabs(token.payload.number - -12.09) < 1e-10);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::ArrayBegin);
    token = tokenizer.next();
    CHECK(token.type == Token::Type::End);
}

TEST(json_tokenizer_invalid)
{
    const char* invalids[] = {
        "m",
        "-",
        "nulln",
        "True",
        "|",
        "\" \\uabcq\"",
        ".4",
        "01",
        "3.4e--5",
        "--4",
        "1.",
        "1ef",
        "9.9 null true false /",
        "false \f"
    };

    size_t n = sizeof(invalids) / sizeof(invalids[0]);
    for (size_t i = 0; i < n; ++i) {
        const char* data = invalids[i];
        size_t size = strlen(invalids[i]);
        Tokenizer tokenizer {data, size};
        while (tokenizer.next().type != Token::Type::Invalid);
        CHECK(true); // The test is just that we get here.
    }
}
