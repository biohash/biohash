#pragma once

#include <cstddef>

namespace biohash {
namespace base64 {

// Calculates the size of the base64 encoded data given the size of the decoded
// data.
size_t encoded_size(size_t decoded_size);

// Calculates the size of the decoded data given the data and size of the
// encoded data.
size_t decoded_size(const unsigned char* encoded_data,  size_t encoded_size);

// Encodes 'decoded_data' and places the result in 'encoded_data'.
// The return value is the size of the encoded data. 'encoded_data' must be
// large enough to hold the result.
size_t encode(const unsigned char* decoded_data,  size_t decoded_size,
              unsigned char* encoded_data);

// Decodes 'encoded_data' of size 'encoded_size' and places the result in
// 'decoded_data'. 'decoded_data' must be large enough to hold the result.
// 'decoded_size' is set to the size of the decoded data.
// The return value is true if and only if the input is valid. If the input is
// invalid, the output should not be used.
bool decode(const unsigned char* encoded_data, size_t encoded_size,
              unsigned char* decoded_data, size_t& decoded_size);
}
}
