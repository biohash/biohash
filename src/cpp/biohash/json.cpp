#include <string.h>
#include <math.h>

#include "json.hpp"
#include "assert.hpp"


using namespace biohash;
using namespace biohash::json;

Tokenizer::Tokenizer(const unsigned char* data, size_t size):
    end {data + size},
    cur {data}
{
}

Token Tokenizer::next()
{
    ASSERT(!invalid);
    ASSERT(!finished);

    while (cur != end && is_whitespace(*cur))
           ++cur;

    if (cur == end) {
        finished = true;
        Token token;
        token.type = Token::Type::End;
        return token;
    }

    if (*cur == 'n')
        return parse_fixed("null", Token::Type::Null);

    if (*cur == 't')
        return parse_fixed("true", Token::Type::True);

    if (*cur == 'f')
        return parse_fixed("false", Token::Type::False);

    if (*cur == '[') {
        Token token;
        token.type = Token::Type::ArrayBegin;
        ++cur;
        return token;
    }

    if (*cur == ']') {
        Token token;
        token.type = Token::Type::ArrayEnd;
        ++cur;
        return token;
    }

    if (*cur == '{') {
        Token token;
        token.type = Token::Type::ObjectBegin;
        ++cur;
        return token;
    }

    if (*cur == '}') {
        Token token;
        token.type = Token::Type::ObjectEnd;
        ++cur;
        return token;
    }

    if (*cur == ',') {
        Token token;
        token.type = Token::Type::Comma;
        ++cur;
        return token;
    }

    if (*cur == '-' || is_digit(*cur))
        return parse_number();

    if (*cur == '"')
        return parse_string();

    invalid = true;
    Token token;
    token.type = Token::Type::Invalid;

    return token;

}

bool Tokenizer::is_whitespace(unsigned char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool Tokenizer::is_digit(unsigned char c)
{
    return c >= '0' && c <= '9';
}

bool Tokenizer::is_hex(unsigned char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

double Tokenizer::to_double(unsigned char c)
{
    return static_cast<double>(c - '0');
}

Token Tokenizer::parse_fixed(const char* fixed, Token::Type type)
{
    ASSERT(*cur == *fixed);
    Token token;

    size_t size = strlen(fixed);
    for (size_t i = 1; i < size; ++i) {
        ++cur;
        if (cur == end || *cur != fixed[i]) {
            invalid = true;
            token.type = Token::Type::Invalid;
            return token;
        }
    }
    ++cur;

    token.type = type;
    return token;
}

Token Tokenizer::parse_number()
{
    ASSERT(*cur == '-' || is_digit(*cur));
    Token token;

    double sign = 1;
    if (*cur == '-') {
        sign = -1;
        ++cur;
    }

    if (cur == end || !is_digit(*cur)) {
        invalid = true;
        token.type = Token::Type::Invalid;
        return token;
    }

    bool leading_zero = (*cur == '0');
    double digit = to_double(*cur);
    ++cur;
    if (leading_zero && cur != end && is_digit(*cur)) {
        invalid = true;
        token.type = Token::Type::Invalid;
        return token;
    }

    long double value = digit;
    while (cur != end && is_digit(*cur)) {
        value = 10 * value + to_double(*cur);
        ++cur;
    }

    if (cur != end && *cur == '.') {
        ++cur;
        if (cur == end || !is_digit(*cur)) {
            invalid = true;
            token.type = Token::Type::Invalid;
            return token;
        }
        long double multiplier = 0.1;
        value += multiplier * to_double(*cur);
        ++cur;
        while (cur != end && is_digit(*cur)) {
            multiplier *= 0.1;
            value += multiplier * to_double(*cur);
            ++cur;
        }
    }

    if (cur != end && (*cur == 'e' || *cur == 'E')) {
        ++cur;
        double esign = 1;
        if (cur != end && (*cur == '+' || *cur == '-')) {
            if (*cur == '-')
                esign = -1;
            ++cur;
        }
        if (cur == end || !is_digit(*cur)) {
            invalid = true;
            token.type = Token::Type::Invalid;
            return token;
        }
        long double evalue = to_double(*cur);
        ++cur;
        while (cur != end && is_digit(*cur)) {
            evalue = 10 * evalue + to_double(*cur);
            ++cur;
        }
        value *= powl(10, esign * evalue);
    }

    token.type = Token::Type::Number;
    token.payload.number = sign * value;
    return token;
}

Token Tokenizer::parse_string()
{
    ASSERT(*cur == '"');
    Token token;

    ++cur;
    if (cur == end) {
        invalid = true;
        token.type = Token::Type::Invalid;
        return token;
    }

    bool escaped = false;
    const unsigned char* str_begin = cur;

    while (cur != end) {
        if (!escaped) {
            unsigned char c = *cur;
            if (c == '"') {
                token.type = Token::Type::String;
                token.payload.string.data = str_begin;
                token.payload.string.size = cur - str_begin;
                ++cur;
                return token;
            }
            else if (c == '\\') {
                escaped = true;
            }
            else if (c == '\t' || c == '\f' || c == '\n' || c == '\r' || c == '\t') {
                invalid = true;
                token.type = Token::Type::Invalid;
                return token;
            }
        }
        else {
            unsigned char c = *cur;
            if (c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f'
                || c == 'n' || c == 'r' || c == 't') {
                escaped = false;
            }
            else if (*cur == 'u') {
                for (int i = 0; i < 4; ++i) {
                    ++cur;
                    if (cur == end || !is_hex(*cur)) {
                        invalid = true;
                        token.type = Token::Type::Invalid;
                        return token;
                    }
                }
                escaped = false;
            }
            else {
                invalid = true;
                token.type = Token::Type::Invalid;
                return token;
            }
        }
        ++cur;
    }

    invalid = true;
    token.type = Token::Type::Invalid;
    return token;
}
