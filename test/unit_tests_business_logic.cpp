//
// Created by Vicram on 9/15/2019.
//

#include <cstring> // std::size_t
#include <cstdint> // uint_fast32_t

#include "../Catch2/single_include/catch2/catch.hpp"

std::size_t construct_escape_string(unsigned char *buf, std::uint_fast32_t codepoint);

TEST_CASE("Test construct_escape_string", "[construct_escape_string]") {
    unsigned char buf[10];
    buf[0] = '\\';
    buf[1] = 'u';
    buf[2] = '\'';

    SECTION("0x0") {
        std::size_t len = construct_escape_string(buf, 0x0);
        REQUIRE(len == 8);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'0000'", len));
    }
    SECTION("0x10") {
        std::size_t len = construct_escape_string(buf, 0X0010);
        REQUIRE(len == 8);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'0010'", len));
    }
    SECTION("0xFFF") {
        std::size_t len = construct_escape_string(buf, 0xFFFu);
        REQUIRE(len == 8);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'0FFF'", len));
    }
    SECTION("0x1000") {
        std::size_t len = construct_escape_string(buf, 0x1000U);
        REQUIRE(len == 8);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'1000'", len));
    }
    SECTION("0xFEDC") {
        std::size_t len = construct_escape_string(buf, 0xFEDC);
        REQUIRE(len == 8);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'FEDC'", len));
    }
    SECTION("0x10000") {
        std::size_t len = construct_escape_string(buf, 0x10000);
        REQUIRE(len == 9);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'10000'", len));
    }
    SECTION("0xC0F58") {
        std::size_t len = construct_escape_string(buf, 0XC0F58);
        REQUIRE(len == 9);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'C0F58'", len));
    }
    SECTION("0xFFFFF") {
        std::size_t len = construct_escape_string(buf, 0xFFFFFU);
        REQUIRE(len == 9);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'FFFFF'", len));
    }
    SECTION("0x100000") {
        std::size_t len = construct_escape_string(buf, 0X100000);
        REQUIRE(len == 10);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'100000'", len));
    }
    SECTION("0x10CAFE") {
        std::size_t len = construct_escape_string(buf, 0x0010CaFeu);
        REQUIRE(len == 10);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'10CAFE'", len));
    }
    SECTION("0x10FFFF") {
        std::size_t len = construct_escape_string(buf, 0X010FFFf);
        REQUIRE(len == 10);
        REQUIRE_FALSE(std::memcmp(buf, "\\u'10FFFF'", len));
    }
}
