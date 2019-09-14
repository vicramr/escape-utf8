//
// Created by Vicram on 9/3/2019.
//

#include <string>
#include <cassert>
#include <cstdint> // uint_fast32_t, uint_fast64_t
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ios>

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
std::size_t construct_escape_string(unsigned char *buf, std::uint_fast32_t codepoint) {
    // Using a solution modified from here: https://stackoverflow.com/a/5100745
    std::stringstream stream;
    if (codepoint < 0xF000u) { // If number is less than 4 chars, pad it
        stream << std::setfill('0') << std::setw(4);
    }
    stream << std::hex << codepoint;
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
 * to the end of the escaped output text. Of course, that's only important if we're writing the
 * escaped text to stdout, because otherwise we wouldn't have the issue that normal output and
 * error text are being printed to the same stream.
 */
void get_stderr_ready() {
    std::cerr << std::endl << std::endl;
}

int read_and_escape(const StreamPair& streams) {
    /*
     * Error Handling
     * We're using 3 I/O functions here: basic_istring::get, basic_ostring::put, and
     * basic_ostring::write. This page contains information on all the ways these functions
     * (and many others) can fail: https://en.cppreference.com/w/cpp/io/ios_base/iostate
     * In short, here's what we need to be able to handle:
     *   basic_istring::get can fail to extract any characters, in which case eofbit and
     *   failbit (but not badbit) will be set.
     *
     *   basic_ostring::put and basic_ostring::write can both fail, and upon any failure,
     *   badbit will be set.
     *   In addition, the basic_ostream::sentry constructor, which is executed at the beginning
     *   of every output function, may fail "under implementation-defined conditions", in which
     *   case the failbit will be set.
     *   Presumably, these two rules, taken together, mean that for basic_ostream, badbit will be
     *   set iff failbit is set, but I don't think we really need to care.
     *
     * The above link also contains a really useful table that specifies how the outputs of
     * functions like good(), fail(), and bad() correspond to badbit, failbit, and eofbit.
     */


    // This will keep track of the number of bytes successfully read. With a 64-bit unsigned int,
    // there is no risk of overflow; 2^64-1 should be significantly larger than the max file size on any
    // system. The nasty type name is just a platform-independent way of saying uint64: https://en.cppreference.com/w/cpp/types/integer
    std::uint_fast64_t num_bytes_read = 0;

    // This buffer will hold the 6-8 character strings for the escaped non-ASCII (or non-printable) chars.
    unsigned char buf[8];
    buf[0] = '\\';
    buf[1] = 'u';
    assert(buf[0] == 92);
    assert(buf[1] == 117);


    for (unsigned char byte = streams.in->get(); streams.in->good() && streams.out->good(); byte = streams.in->get()) {
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
        // that's at least 21 bits wide.
        std::uint_fast32_t decoded_char = 0;

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
            get_stderr_ready();
            std::cerr << "The given text is not valid UTF-8 text. Exiting now." << std::endl;
            // TODO: I've commented out several error messages because I didn't want to write tests for them. Might do that at some point.
//            std::cerr << "Byte " << num_bytes_read << " is invalid: 0x" << std::hex << ((unsigned int)byte) <<
//            " is not valid as the first character in a UTF-8 character." << std::endl;
            return 1;
        }
        // First we grab the 5, 4, or 3 relevant bits from the first byte and get them into position in decoded_char.
        decoded_char = (byte & mask);
        // Next, read the rest of the bytes, check that the first two bits are '10', and grab the last 6 bits.
        for (int i = 1; i < numbytes; ++i) { // we start at 1 because we've already read the first byte.
            byte = streams.in->get();
            if (!streams.in->good()) {
                // As mentioned above, the only valid combination of state flags is
                // eofbit, failbit, and not badbit.
                if (streams.in->eof() && streams.in->fail() && (!streams.in->bad())) {
                    get_stderr_ready();
                    std::cerr << "The given text is not valid UTF-8 text. Exiting now." << std::endl;
//                    std::cerr << "Reached EOF after reading " << num_bytes_read <<
//                    " byte(s). The given text is NOT valid UTF-8 text because it stopped in the middle of a multi-byte UTF-8 character."
//                    << std::endl;
                    return 1;
                } else {
                    get_stderr_ready();
                    std::cerr << "Failed when trying to read byte " << (num_bytes_read + 1) << " due to unknown error." << std::endl;
                    return 2;
                }

            }
            ++num_bytes_read;
            if ((byte & 0b11000000u) != 0b10000000u) {
                // The first two bits aren't '10' so the file is not valid UTF-8
                get_stderr_ready();
                std::cerr << "The given text is not valid UTF-8 text. Exiting now." << std::endl;
//                std::string ordinal;
//                if (i == 1) {
//                    ordinal = "second";
//                } else if (i == 2) {
//                    ordinal = "third";
//                } else {
//                    assert(i == 3);
//                    ordinal = "fourth";
//                }
//                std::cerr << "The given text is not valid UTF-8 text. Byte " << num_bytes_read <<
//                " was supposed to be the " << ordinal << " byte in a multi-byte UTF-8 character but it is not in the valid range."
//                << std::endl;
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
            get_stderr_ready();
            std::cerr << "The given text is not valid UTF-8 text. Exiting now." << std::endl;
//            std::cerr << "The given text is not valid UTF-8 text. "
//            << "Bytes " << (numbytes-1) << " and " << numbytes << " form a 2-byte UTF-8 character with value 0x"
//            << std::hex << decoded_char << "which is outside the valid range of [0x80, 0x7FF]." << std::endl;
            return 1;
        }
        if (numbytes == 3 && (decoded_char < 0x800u || decoded_char > 0xFFFFu)) {
            get_stderr_ready();
            std::cerr << "The given text is not valid UTF-8 text. Exiting now." << std::endl;
//            std::cerr << "The given text is not valid UTF-8 text. "
//            << "Bytes " << (numbytes-2) << ", " << (numbytes-1) << ", and " << numbytes << " form a 3-byte UTF-8 character with value 0x"
//            << std::hex << decoded_char << "which is outside the valid range of [0x800, 0xFFFF]." << std::endl;
            return 1;
        }
        if (numbytes == 4 && (decoded_char < 0x10000u || decoded_char > 0x10FFFFu)) {
            get_stderr_ready();
            std::cerr << "The given text is not valid UTF-8 text. Exiting now." << std::endl;
//            std::cerr << "The given text is not valid UTF-8 text. "
//            << "Bytes " << (numbytes-3) << " through " << numbytes << " form a 4-byte UTF-8 character with value 0x"
//            << std::hex << decoded_char << "which is outside the valid range of [0x10000, 0x10FFFF]." << std::endl;
            return 1;
        }

        // Finally, we can print out the escaped character and move on.
        std::size_t buflen = construct_escape_string(buf, decoded_char);
        streams.out->write((char *)buf, buflen);
        // We're only printing out ASCII chars so interpreting them as signed should be fine,
        // because all ASCII values are representable by a signed 8-bit int.
    }
    // Here we need to check whether we exited the loop due to EOF or something else.
    // There are no break statements, so if we got here, that implies we failed the
    // check at the beginning of every loop. Additionally, every single possible path
    // through the loop which doesn't result in a return statement ends with both a
    // write operation (from streams.out->put or streams.out->write) and a read operation
    // (from the streams.in->get in the for statement).
    // The upshot of this is that no matter what, we need to check the status of both streams.

    if (streams.out->fail()) {
        get_stderr_ready();
        std::cerr << "There was a fatal error when trying to write to the output. Exiting now." << std::endl;
        return 3;
    }
    if (streams.in->eof() && streams.in->fail() && (!streams.in->bad())) {
        // This is the success case: exited because we reached EOF, and it wasn't
        // in the middle of a character.
        return 0;
    } else {
        get_stderr_ready();
        std::cerr << "Failed when trying to read byte " << (num_bytes_read + 1) << " due to unknown error." << std::endl;
        return 2;
    }

}
