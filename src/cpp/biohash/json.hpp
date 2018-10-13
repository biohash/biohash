#pragma once

#include <stddef.h>

namespace biohash {
namespace json {

struct Token {

    enum class Type {
        Invalid = 0,
        Null,
        True,
        False,
        Number,
        String,
        ArrayBegin,
        ArrayEnd,
        ObjectBegin,
        ObjectEnd,
        Comma,
        End
    };

    Type type;
    union {
        long double number;
        struct {
            const unsigned char* data;
            size_t size;
        } string;

    } payload;
};

class Tokenizer {
public:

    Tokenizer(const unsigned char* data, size_t size);

    // The Tokenizer object keeps track of progress in cursor. By calling
    // next(), the next token will be emitted. When the end has been reached,
    // next() will emit the End token. If next() encounters an invalid input,
    // it will return the Invalid token. It is an error to call next() after
    // End or Invalid has been seen.
    Token next();

private:
    const unsigned char* const end;
    const unsigned char* cur;

    bool finished = false;
    bool invalid = false;

    static bool is_whitespace(unsigned char c);
    static bool is_digit(unsigned char c);
    static bool is_hex(unsigned char c);
    static double to_double(unsigned char c);

    Token parse_fixed(const char* fixed, Token::Type type);
    Token parse_number();
    Token parse_string();
};

}
}
