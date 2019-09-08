//
// Created by Vicram on 9/3/2019.
//

#include <string>
#include <cassert>
#include <cstdint> // uint_fast32_t
#include <iostream>
#include <sstream>
#include <iomanip>

#include "business_logic.h"

// Note: binary literals were only added in C++14 so this means that support for C++14 is required.

/*
 * These are used to check the top few bits of the first byte of a multi-byte UTF-8 character,
 * in order to figure out how many bytes are in it.
 * Usage: iff IS_X_BYTES(byte) evaluates to true, then the character has X bytes.
 */
#define TWO_BYTE_MASK   0b11100000u
#define TWO_BYTE_VAL    0b11000000u
#define IS_TWO_BYTES(x) ((TWO_BYTE_MASK & x) == TWO_BYTE_VAL)

#define THREE_BYTE_MASK 0b11110000u
#define THREE_BYTE_VAL  0b11100000u
#define IS_THREE_BYTES(x) ((THREE_BYTE_MASK & x) == THREE_BYTE_VAL)

#define FOUR_BYTE_MASK  0b11111000u
#define FOUR_BYTE_VAL   0b11110000u
#define IS_FOUR_BYTES(x) ((FOUR_BYTE_MASK & x) == FOUR_BYTE_VAL)

/**
 * Given a buffer and a Unicode code point, this function constructs the escape
 * string for that code point. For example, given the code point 1000 (hex value 0x3E8)
 * this function would write to the buffer "\u03e8".
 *
 * Any number whose hex representation would fit in less than 4 characters (like 0x3E8)
 * will be padded by leading zeros to reach 4 characters. No padding is done for larger
 * values.
 * @param buf A buffer of length 8 bytes, where the first two bytes are "\u".
 * @param codepoint Int representing a Unicode code point; must be a number in [0, 2^21).
 * @return The number of characters in the escape string. Will be 6, 7, or 8.
 */
std::size_t construct_escape_string(unsigned char *buf, uint_fast32_t codepoint) {
    // Using a solution modified from here: https://stackoverflow.com/a/5100745
    std::stringstream stream;
    if (codepoint < 0xF000u) { // If number is less than 4 chars, pad it
        stream << std::setfill('0') << std::setw(4);
    }
    stream << codepoint;
    std::string result(stream.str()); // TODO can we optimize out the copy done by stream.str()?
    assert(result.length() >= 4 && result.length() <= 6);
    // Finally, copy the hex string over to the buffer.
    for (std::size_t i = 0; i < result.length(); ++i) {
        buf[i + 2] = result[i];
    }
    return result.length() + 2;
}

/**
 * This function will print out some newlines to stderr. Its only purpose is to make sure
 * that when the end user gets their error message, it's on a new line rather than concatenated
 * to the end of the escaped output text.
 */
void get_stderr_ready() {
    std::cerr << std::endl << std::endl;
}

int read_and_escape(const StreamPair& streams) {
    std::size_t num_bytes_read = 0;

    // This buffer will hold the 6-8 character strings for the escaped non-ASCII (or non-printable) chars.
    unsigned char buf[8];
    buf[0] = '\\';
    buf[1] = 'u';
    assert(buf[0] == 92);
    assert(buf[1] == 117);

    unsigned char byte = streams.in->get();
    while (streams.in->good() && streams.out->good()) {
        ++num_bytes_read; // Mark the byte as read (we have to check good() first)
        // First, we'll check if the character is printable. This includes 33-126
        // (all normal graphical characters) plus 9 (tab), 10 (line feed), and 32 (space).
        // These numbers are all in decimal.

        if ((32 <= byte && byte <= 126) || byte == 9 || byte == 10) {
            streams.out->put(byte);
            continue;
        }
        if (byte == 127) { // DEL, U+007F
            streams.out->write("\\u007f", 6);
            continue;
        }
        // Otherwise we're dealing with a multi-byte character.

        // This will store the decoded character's actual numerical value. We need an int
        // that's at least 21 bits wide. The nasty type name is just a platform-independent
        // way of saying uint32: https://en.cppreference.com/w/cpp/types/integer
        uint_fast32_t decoded_char = 0;

        // The first byte tells us how many bytes the character is.
        int numbytes;
        unsigned char mask; // Used to read only the relevant low-order bits from the first byte.
        if (IS_TWO_BYTES(byte)) {
            numbytes = 2;
            mask = 0b00011111u;
        } else if (IS_THREE_BYTES(byte)) {
            numbytes = 3;
            mask = 0b00001111u;
        } else if (IS_FOUR_BYTES(byte)) {
            numbytes = 4;
            mask = 0b00000111u;
        } else {
            // TODO print error message
            return 1;
        }
        // First we grab the 5, 4, or 3 relevant bits from the first byte and get them into position in decoded_char.
        decoded_char = (byte & mask);
        // Next, read the rest of the bytes, check that the first two bytes are '10', and grab the last 6 bits.
        for (int i = 0; i < numbytes; ++i) {
            byte = streams.in->get();
            if (!streams.in->good()) {
                // TODO handle error
                return 1;
            }
            if ((byte & 0b11000000u) != 0b10000000u) {
                // The first two bits aren't '10' so the file is not valid UTF-8
                //TODO handle error
                return 1;
            }
            decoded_char <<= 6u; // TODO might be faster to instead shift each bit to its final position when assigning
            decoded_char |= ((uint_fast32_t)(byte & 0b00111111u));
        }
        // We've now constructed an int with the numeric value of the Unicode character.
        // However, we still need to check that the value is within the appropriate range for
        // a 2- or 3- or 4-byte UTF-8 character. See page 5 of RFC 3629, where they discuss "a naive implementation":
        // https://tools.ietf.org/html/rfc3629
        if (numbytes == 2 && (decoded_char < 0x80u || decoded_char > 0x7FFu)) {
            // TODO print error message
            return 1;
        }
        if (numbytes == 3 && (decoded_char < 0x800u || decoded_char > 0xFFFFu)) {
            // TODO print error message
            return 1;
        }
        if (numbytes == 4 && (decoded_char < 0x10000u || decoded_char > 0x10FFFFu)) {
            // TODO print error message
            return 1;
        }

        // Finally, we can print out the escaped character and move on.
        std::size_t buflen = construct_escape_string(buf, decoded_char);
        streams.out->write((char *)buf, buflen);
    }
    // TODO check whether we exited due to EOF or something else
    return 0;
}
