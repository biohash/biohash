#include <string.h>
#include <stdlib.h>

#include "util/test.hpp"
#include <biohash/base64.hpp>
#include <biohash/assert.hpp>

using namespace biohash;
using namespace biohash::test;

namespace {

struct Pair {
    const char* decoded_data;
    const size_t decoded_size;
    const char* encoded_data;
};

Pair pairs[] = {
    {"", 0, ""},
    {"\0", 1, "AA=="},
    {"\1", 1, "AQ=="},
    {"\2", 1, "Ag=="},
    {"\x6a", 1, "ag=="},
    {"\x64", 1, "ZA=="},
    {"\xff", 1, "/w=="},
    {"\1\1", 2, "AQE="},
    {"\x64\x64", 2, "ZGQ="},
    {"\xff\xff", 2, "//8="},
    {"\0\0\0", 3, "AAAA"},
    {"\1\1\1", 3, "AQEB"},
};

const char* invalids[] = {
    "a",
    "ab",
    "abc",
    "abc-",
    "%bcd",
    "ab=c",
    "a===",
    "=b==",
    "AB==",
    "Af==",
    "+o==",
    "AAB=",
    "34/=",
    "12345",
    "1234=",
    "123456",
    "abcdefg",
    "abcd12=3",
    "///////="
};

}

#include <iostream>

TEST(base64_encode)
{
    char result[10];
    size_t npairs = sizeof(pairs) / sizeof(Pair);
    for (size_t i = 0; i < npairs; ++i) {
        const Pair& pair = pairs[i];
        size_t decoded_size = pair.decoded_size;
        const char* decoded_data = pair.decoded_data;
        size_t encoded_size = strlen(pair.encoded_data);
        size_t encoded_size_0 = base64::encoded_size(decoded_size);
        CHECK_EQUAL(encoded_size_0, encoded_size);
        ASSERT(encoded_size < 10);
        size_t encoded_size_1 = base64::encode(decoded_data, decoded_size, result);
        CHECK_EQUAL(encoded_size_1, encoded_size);
        CHECK(memcmp(pair.encoded_data, result, encoded_size) == 0);
    }
}

TEST(base64_decode)
{
    char result[10];
    size_t npairs = sizeof(pairs) / sizeof(Pair);
    for (size_t i = 0; i < npairs; ++i) {
        const Pair& pair = pairs[i];
        const char* encoded_data = pair.encoded_data;
        size_t encoded_size = strlen(pair.encoded_data);
        size_t decoded_size = pair.decoded_size;
        size_t decoded_size_0 = base64::decoded_size(encoded_data, encoded_size);
        CHECK_EQUAL(decoded_size, decoded_size_0);
        size_t result_size;
        bool ret = base64::decode(encoded_data, encoded_size, result, result_size);
        CHECK(ret);
        CHECK_EQUAL(decoded_size, result_size);
        CHECK(memcmp(pair.decoded_data, result, decoded_size) == 0);
    }
}

TEST(base64_invalid)
{
    size_t num_invalids = sizeof(invalids) / sizeof(char*);

    for (size_t i = 0; i < num_invalids; ++i) {
        const char* encoded_data = invalids[i];
        const size_t encoded_size = std::strlen(encoded_data);
        ASSERT(encoded_size < 20);
        char decoded_data[30];
        size_t decoded_size;
        bool ret = base64::decode(encoded_data, encoded_size,
                                  decoded_data, decoded_size);
        CHECK(!ret);
    }
}

TEST(base64_random)
{
    char decoded_data_0[30];
    char decoded_data_1[30];
    char encoded_data[40];

    for (int i = 0; i < 20; ++i) {
        const size_t decoded_size_0 = arc4random_uniform(31);
        ASSERT(decoded_size_0 <= 30);
        arc4random_buf(decoded_data_0, decoded_size_0);
        size_t encoded_size_0 = base64::encoded_size(decoded_size_0);
        ASSERT(encoded_size_0 <= 40);
        size_t encoded_size_1 = base64::encode(decoded_data_0, decoded_size_0, encoded_data);
        CHECK_EQUAL(encoded_size_0, encoded_size_1);
        size_t decoded_size_1 = base64::decoded_size(encoded_data, encoded_size_0);
        CHECK_EQUAL(decoded_size_0, decoded_size_1);
        size_t decoded_size_2;
        bool ret = base64::decode(encoded_data, encoded_size_0, decoded_data_1, decoded_size_2);
        CHECK(ret);
        CHECK_EQUAL(decoded_size_0, decoded_size_2);
        CHECK(memcmp(decoded_data_0, decoded_data_1, decoded_size_0) == 0);
    }
}
