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
            const char* data;
            size_t size;
        } string;

    } payload;
};

class Tokenizer {
public:

    Tokenizer(const char* data, size_t size);

    // The Tokenizer object keeps track of progress in cursor. By calling
    // next(), the next token will be emitted. When the end has been reached,
    // next() will emit the End token. If next() encounters an invalid input,
    // it will return the Invalid token. It is an error to call next() after
    // End or Invalid has been seen.
    Token next();

private:
    const char* const end;
    const char* cur;

    bool finished = false;
    bool invalid = false;

    static bool is_whitespace(char c);
    static bool is_digit(char c);
    static bool is_hex(char c);
    static double to_double(char c);

    Token parse_fixed(const char* fixed, Token::Type type);
    Token parse_number();
    Token parse_string();
};

}
}
