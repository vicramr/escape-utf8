//
// Created by Vicram on 9/8/2019.
//

/*
 * This file contains tests for the helper functions in parseargs.cpp.
 */
#include <cstring> // std::size_t

#include "../Catch2/single_include/catch2/catch.hpp"

// Function prototypes for functions that aren't exposed through the headers
bool strlen_atleast(const char *str, std::size_t len);
int check_output_option(const char *arg);

TEST_CASE("Test strlen_atleast", "[strlen_atleast]") {
    REQUIRE(strlen_atleast("foo", 0));
    REQUIRE(strlen_atleast("foo", 1));
    REQUIRE(strlen_atleast("foo", 2));
    REQUIRE(strlen_atleast("foo", 3));
    REQUIRE_FALSE(strlen_atleast("foo", 4));
    REQUIRE_FALSE(strlen_atleast("foo", 10000));
    REQUIRE(strlen_atleast("\n\n\n\n", 4));
    REQUIRE_FALSE(strlen_atleast("\n\n\n\n", 5));
    REQUIRE(strlen_atleast("", 0));
    REQUIRE_FALSE(strlen_atleast("", 1));
    REQUIRE(strlen_atleast("abc\0def", 3));
    REQUIRE_FALSE(strlen_atleast("abc\0def", 4));
}

TEST_CASE("Test check_output_option", "[check_output_option]") {
    REQUIRE(check_output_option("-o") == 1);
    REQUIRE(check_output_option("--output=") == 0);
    REQUIRE(check_output_option("-ofoo") == 2);
    REQUIRE(check_output_option("-o foo") == 2);
    REQUIRE(check_output_option("--output=foo") == 9);
    REQUIRE(check_output_option("--output=\n") == 9);
    REQUIRE(check_output_option("") == -1);
    REQUIRE(check_output_option("-O") == -1);
    REQUIRE(check_output_option("--output") == 1);
    REQUIRE(check_output_option("-\no") == -1);
    REQUIRE(check_output_option("foo") == -1);
}
