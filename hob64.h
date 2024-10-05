/*
Copyright (c) 2024 Luke Philipsen

Permission to use, copy, modify, and/or distribute this software for
any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/* Usage

  Do this:
    #define HOB64_IMPLEMENTATION
  before you include this file in *one* C or C++ file to create the implementation.

  You can define HOB64_DECL with
    #define HOB64_DECL static
  or
    #define HOB64_DECL extern
  to specify HOB64 function declarations as static or extern, respectively.
  The default specifier is extern.
*/

#ifndef HOB64_H
    #define HOB64_H

#include <stddef.h> /* NULL, size_t */
#include <stdlib.h> /* malloc() */
#include <stdint.h> /* uint32_t */

#ifndef HOB64_DECL
    #define HOB64_DECL
#endif /* HOB64_DECL */

#ifdef __cplusplus
    extern "C" {
#endif /* __cpluspus */

/***************/
/* Definitions */

/**
 * Decode the given Base64-encoded string of the given length. The final parameter is an output and is assigned with the
 * length of the decoded data that is returned, or 0 if decoding isn't possible.
 * The returned array is allocated by this function and should be freed later.
 *
 * @param encoded A Base64-encoded string to be decoded as binary data.
 * @param encodedLength The length of the encoded string.
 * @param decodedLength Output parameter assigned with the length, in 8-bit bytes, of the decoded data.
 * @return The binary data originally encoded as Base64 text, or NULL if decoding wasn't possible.
 */
HOB64_DECL unsigned char* hob64_decode(const char* encoded, size_t encodedLength, size_t* decodedLength);

/**
 * Encode the given binary data of the given lenght as a Base64 string. The final parameter is an output and is assigned
 * with the length of the encoded string that is returned, including padding, or 0 if encoding isn't possible.
 * The returned string is allocated by this function and should be freed later.
 *
 * @param data Binary data to be encoded as a Base64 string.
 * @param dataLength The length of the data in (8-bit) bytes.
 * @param encodedLength Output parameter assigned with the length of the encoded string, including padding.
 * @return The Base64-encoded string equivalent of the given binary data, or NULL if encoding wasn't possible.
 */
HOB64_DECL char* hob64_encode(const unsigned char* data, size_t dataLength, size_t* encodedLength);

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* HOB64_H */

#ifdef HOB64_IMPLEMENTATION

/******************/
/* Implementation */

unsigned char* hob64_decode(const char* encoded, const size_t encodedLength, size_t* decodedLength) {
    /* Base64 data is encoded by replacing specific 6-bit digits with an ASCII character with the digit-to-ASCII */
    /* mapping defined in RFC 4648. The table below is the inverse, an ASCII-to-digit mapping where the encoded */
    /* <ASCII character> - 43 is used as the index. For example, bits 111110 (value 62) are encoded as the '+' ASCII */
    /* character. Using this table, 'decodeTable[+ - 43]' will get the digit at index zero, 62 (111110). The indexes */
    /* with value 255 are just fillers; <ASCII character> - 43 will never result in those indexes. */
    static unsigned char decodeTable[] = {
        62, 255, 255, 255, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255, 255, 0, 1, 2, 3, 4,
        5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255, 255, 26,
        27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };

    /* Quick input sanitation to confirm a string was actually provided */
    if (encoded == NULL || encodedLength <= 0) { /* If there is no string to decode */
        *decodedLength = 0;
        return NULL;
    }

    /* Check the encoded string for valid characters */
    size_t i;
    for (i = 0; i < encodedLength; i++) {
        /* Base64-encoded strings can use characters 'A' to 'Z', 'a' to 'z', '0' to '0', '+', '/', and '='. Any other */
        /* characters are invalid and could cause undefined behavior while attempting to decode them. */
        char character = encoded[i];
        int isValid =  0;
        if (character >= 'A' && character <= 'Z')
            isValid = 1;
        else if (character >= 'a' && character <= 'z')
            isValid = 1;
        else if (character >= '0' && character <= '9')
            isValid = 1;
        else if (character == '+' || character == '/' || character == '=')
            isValid =  1;

        if (!isValid) {
            *decodedLength = 0;
            return NULL;
        }
    }

    /* Sort-of bounds check. When encoding, every 24 bits ends up ended as 32 bits (four bytes). If the encoded */
    /* string's length is not a multiple of four, some amount of those 24 bits is missing; the original data cannot */
    /* be reproduced and the decoding algorithm here will end up trying to decode adjacent memory. Cannot continue. */
    if (encodedLength % 4 != 0) { /* If the encoded string is an unusable length */
        *decodedLength = 0;
        return NULL;
    }

    /* Calculate the length of the decoded data. This is mostly straightforward as each character of the input maps */
    /* to six bits with the exception of '=' (padding, not decoded). In other words, 32 bits input = 24 bits output. */
    *decodedLength = encodedLength / 4 * 3; /* Decoded length = 3/4 encoded length because 24/32 = 3/4 */
    if (encoded[encodedLength - 1] == '=')
        *decodedLength -= 1; /* Subtract eight bits (assumed to be a byte) because '=' is padding, not data */
    if (encoded[encodedLength - 2] == '=')
        *decodedLength -= 1;

    unsigned char* decoded = (unsigned char*)malloc((unsigned int)*decodedLength);
    if (decoded == NULL) { /* If memory allocation failed */
        *decodedLength = 0;
        return NULL;
    }

    /* Iterate through each (ASCII) character in the encoded string */
    size_t j;
    for (i = 0, j = 0; i < encodedLength && j < *decodedLength;) {
        /* Map four of the encoded characters to four sextets (6-bit digits). '=' is padding so it maps to zero. */
        /* Note: '0 & i++' evaluates to integer value zero while incrementing 'i' by one */
        uint32_t sextet1 = (encoded[i] == '=') ? (0 & i++) : (decodeTable[(int)encoded[i++] - 43]);
        uint32_t sextet2 = (encoded[i] == '=') ? (0 & i++) : (decodeTable[(int)encoded[i++] - 43]);
        uint32_t sextet3 = (encoded[i] == '=') ? (0 & i++) : (decodeTable[(int)encoded[i++] - 43]);
        uint32_t sextet4 = (encoded[i] == '=') ? (0 & i++) : (decodeTable[(int)encoded[i++] - 43]);
        /* Concatenate the four sextets and store them. Although the variable is 32 bits, only 24 will be used. */
        uint32_t concatenated = (sextet1 << 18) + (sextet2 << 12) + (sextet3 << 6) + (sextet4 << 0);
        /* Copy the three octets (three 8-bit digits, 24 bits total) of the concatenated integer to the output buffer */
        if (j < *decodedLength)
            decoded[j++] = (concatenated >> 16) & 0xFF;
        if (j < *decodedLength)
            decoded[j++] = (concatenated >> 8) & 0xFF;
        if (j < *decodedLength)
            decoded[j++] = (concatenated >> 0) & 0xFF;
    }

    return decoded;
}

char* hob64_encode(const unsigned char* data, const size_t dataLength, size_t* encodedLength) {
    /* Base64 data is encoded by replacing specific 6-bit digits with an ASCII character with the digit-to-ASCII */
    /* mapping defined in RFC 4648. The table below maps those values with the ASCII charcters that replace them. For */
    /* example, bits 000011 (value 3) are encoded as the 'D' ASCII cahrcter. Using this table, 'encodeTable[3]' will */
    /* get the character replacing the bits with value 3 (000011), 'D'. */
    static char encodeTable[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
        's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
    };

    /* Quick input sanitation to confirm data was actually provided */
    if (data == NULL || dataLength <= 0) { /* If there is no data to encode */
        *encodedLength = 0;
        return NULL;
    }

    /* Calculate the length of the encoded data. Each six bits of input correspond to eight bits of output. In other */
    /* words, 24 bits input = 32 bits output. */
    /* Note: The + 2 is an integer division trick that leads to rounding up to the nearest multiple of three */
    *encodedLength = 4 * ((dataLength + 2) / 3); /* Encoded length = 4/3 decoded length because 32/24 = 4/3 */

    char* encoded = (char*)malloc(*encodedLength + 1); /* + 1 for the null terminator */
    if (encoded == NULL) {
        *encodedLength = 0;
        return NULL;
    }

    /* Iterate through each 6-bit digit (sextet) in the data. Iteration is done in 24-bit steps simply due the */
    /* typical smallest addressable unit of memory (a "byte") being eight bits. */
    size_t i, j;
    for (i = 0, j = 0; i < dataLength && j < *encodedLength + 1; i += 3, j += 4) {
        /* Map three of the sextets to three Base64-equivalent octect characters */
        uint32_t octet1 = i + 0 < dataLength ? (unsigned char)data[i + 0] : 0;
        uint32_t octet2 = i + 1 < dataLength ? (unsigned char)data[i + 1] : 0;
        uint32_t octet3 = i + 2 < dataLength ? (unsigned char)data[i + 2] : 0;
        /* Concatenate the three characters and store them. Although the variable is 32 bits, only 24 will be used. */
        uint32_t concatenated = (octet1 << 16) + (octet2 << 8) + (octet3 << 0);

        encoded[j + 0] = encodeTable[(concatenated >> 18) & 0x3F];
        encoded[j + 1] = encodeTable[(concatenated >> 12) & 0x3F];
        encoded[j + 2] = i + 1 < dataLength ? encodeTable[(concatenated >> 6) & 0x3F] : '=';
        encoded[j + 3] = i + 2 < dataLength ? encodeTable[(concatenated >> 0) & 0x3F] : '=';
    }

    /* Terminate the encoded string */
    encoded[*encodedLength] = '\0';

    return encoded;
}

#endif /* HOB64_IMPLEMENTATION */
