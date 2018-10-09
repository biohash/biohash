#include <climits>

#include "base64.hpp"
#include <biohash/assert.hpp>

using namespace biohash;

static const unsigned char characters[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S' ,'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+' , '/'
};

static const char pad = '=';

static unsigned char character_to_index(unsigned char ch)
{
    if (ch >= 'A' && ch <= 'Z')
        return ch - 'A';

    if (ch >= 'a' && ch <= 'z')
        return ch - 'a' + 26;

    if (ch >= '0' && ch <= '9')
        return ch - '0' + 52;

    if (ch == '+')
        return 62;

    if (ch == '/')
        return 63;

    return 255;
}

size_t base64::encoded_size(size_t decoded_size)
{
    return ((decoded_size + 2) / 3) * 4;
}

size_t base64::decoded_size(const unsigned char* encoded_data,  size_t encoded_size)
{
    if (encoded_size == 0)
        return 0;

    ASSERT(encoded_size % 4 == 0);
    size_t npads = (encoded_data[encoded_size - 1] == pad ? 1 : 0) +
        (encoded_data[encoded_size - 2] == pad ? 1 : 0);
    return 3 * (encoded_size / 4) - npads;
}

size_t base64::encode(const unsigned char* decoded_data,  size_t decoded_size,
              unsigned char* encoded_data)
{
    ASSERT(CHAR_BIT == 8);
    size_t quot = decoded_size / 3;
    size_t rem = decoded_size % 3;
    size_t ndx_in = 0;
    size_t ndx_out = 0;
    for (size_t i = 0; i < quot; ++i) {
        unsigned char byte_0 = decoded_data[ndx_in++];
        unsigned char byte_1 = decoded_data[ndx_in++];
        unsigned char byte_2 = decoded_data[ndx_in++];
        unsigned char ndx_0 = byte_0 >> 2;
        unsigned char ndx_1 = ((byte_0 & 0x03) << 4) | (byte_1 >> 4);
        unsigned char ndx_2 = ((byte_1 & 0x0f) << 2) | (byte_2 >> 6);
        unsigned char ndx_3 = byte_2 & 0x3f;
        encoded_data[ndx_out++] = characters[ndx_0];
        encoded_data[ndx_out++] = characters[ndx_1];
        encoded_data[ndx_out++] = characters[ndx_2];
        encoded_data[ndx_out++] = characters[ndx_3];
    }
    if (rem != 0) {
        unsigned char byte_0 = decoded_data[ndx_in++];
        unsigned char byte_1 = rem == 2 ? decoded_data[ndx_in++] : 0;
        unsigned char ndx_0 = byte_0 >> 2;
        unsigned char ndx_1 = ((byte_0 & 0x03) << 4) | (byte_1 >> 4);
        encoded_data[ndx_out++] = characters[ndx_0];
        encoded_data[ndx_out++] = characters[ndx_1];
        if (rem == 2) {
            unsigned char ndx_2 = (byte_1 & 0x0f) << 2;
            encoded_data[ndx_out++] = characters[ndx_2];
        }
        else {
            encoded_data[ndx_out++] = pad;
        }
        encoded_data[ndx_out++] = pad;
    }
    ASSERT(ndx_in == decoded_size);
    ASSERT(ndx_out == 4 * quot + (rem != 0 ? 4 : 0));
    return ndx_out;
}

bool base64::decode(const unsigned char* encoded_data, size_t encoded_size,
                    unsigned char* decoded_data, size_t& decoded_size)
{
    ASSERT(CHAR_BIT == 8);
    if (encoded_size == 0) {
        decoded_size = 0;
        return true;
    }
    if (encoded_size % 4 != 0)
        return false;
    bool has_pad = encoded_data[encoded_size - 1] == pad;
    size_t triplets = encoded_size / 4 - (has_pad ? 1 : 0);
    size_t ndx_in = 0;
    size_t ndx_out = 0;
    for (size_t i = 0; i < triplets; ++i) {
        unsigned char ndx_0 = character_to_index(encoded_data[ndx_in++]);
        unsigned char ndx_1 = character_to_index(encoded_data[ndx_in++]);
        unsigned char ndx_2 = character_to_index(encoded_data[ndx_in++]);
        unsigned char ndx_3 = character_to_index(encoded_data[ndx_in++]);
        if (ndx_0 == 255 || ndx_1 == 255 || ndx_2 == 255 || ndx_3 == 255)
            return false;
        unsigned char byte_0 = (ndx_0 << 2) | (ndx_1 >> 4);
        unsigned char byte_1 = ((ndx_1 & 0x0f) << 4) | (ndx_2 >> 2);
        unsigned char byte_2 = ((ndx_2 & 0x03) << 6) | ndx_3;
        decoded_data[ndx_out++] = byte_0;
        decoded_data[ndx_out++] = byte_1;
        decoded_data[ndx_out++] = byte_2;
    }
    if (has_pad) {
        unsigned char ndx_0 = character_to_index(encoded_data[ndx_in++]);
        unsigned char ndx_1 = character_to_index(encoded_data[ndx_in++]);
        if (ndx_0 == 255 || ndx_1 == 255)
            return false;
        unsigned char byte_0 = (ndx_0 << 2) | (ndx_1 >> 4);
        decoded_data[ndx_out++] = byte_0;
        unsigned char ch_2 = encoded_data[ndx_in++];
        ASSERT(encoded_data[ndx_in++] == pad);
        if (ch_2 != pad) {
            unsigned char ndx_2 = character_to_index(ch_2);
            if ((ndx_2 & 0x03) != 0)
                return false;
            unsigned char byte_1 = ((ndx_1 & 0x0f) << 4) | (ndx_2 >> 2);
            decoded_data[ndx_out++] = byte_1;
        }
        else {
            if ((ndx_1 & 0x0f) != 0)
                return false;
        }
    }
    ASSERT(ndx_in == encoded_size);
    ASSERT(ndx_out == base64::decoded_size(encoded_data, encoded_size));
    decoded_size = ndx_out;
    return true;
}
