//
// Created by Vicram on 8/27/2019.
//

#include <iostream>
#include <bitset>
#include <cassert>
#include <cstring> // std::size_t and std::strncmp

#include "parseargs.h"
#include "../version.h"

// This macro is used to identify Windows. Sources:
// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/64-bit-compiler#predefined-macros
// https://sourceforge.net/p/predef/wiki/OperatingSystems/
// https://stackoverflow.com/a/6649992
#ifdef _WIN32
// These headers are for _setmode and _fileno
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#endif


static std::string const helpmsg(
"escape-utf8: Transform UTF-8 text to a representation in ASCII.\n"
"This program takes as input a piece of text encoded in UTF-8, either\n"
"from a file or stdin. It outputs the same text except with any non-\n"
"ASCII characters (and some non-printable ASCII characters) replaced\n"
"with an ASCII representation.\n"
"\n"
"If the input text is not valid UTF-8 text then this program will print\n"
"an error message and terminate.\n"
"\n"
"\n"
"Usage:\n"
"  escape [INPUTFILE] [-o OUTPUTFILE]\n"
"  escape -h | --help\n"
"  escape -v | --version\n"
"\n"
"Argument:\n"
"  INPUTFILE    Path to the input file. This argument may be omitted; if\n"
"               so, input will be read from stdin.\n"
"\n"
"Options:\n"
"  -h, --help                          Show this help message.\n"
"  -v, --version                       Show version.\n"
"  -o OUTPUTFILE, --output OUTPUTFILE  Path to the file to write output\n"
"                                      to. If this option is omitted,\n"
"                                      the output will be printed to\n"
"                                      stdout. If the file doesn't exist\n"
"                                      then it will be created; if it\n"
"                                      does exist, it will be\n"
"                                      overwritten.\n"
);


std::bitset<3> parse_helper(int argc, char **argv, std::string& inputfile, std::string& outputfile);
bool strlen_atleast(const char *str, std::size_t len);
int check_output_option(const char *arg);


StreamPair parse(int argc, char **argv) {
    /*
     * There are 4 potential invocations of this program: help, version,
     * neither (which is valid), and invalid.
     */

    std::string inputfile; // If an arg is given, it's guaranteed to have length > 0, so the empty string serves as our "null" value.
    std::string outputfile;
    std::bitset<3> bits = parse_helper(argc, argv, inputfile, outputfile);
    // Bit 0 is "help", bit 1 is "version", bit 2 is "valid"
    if (bits[1]) {
        assert(bits[2]);
        std::cout << "escape-utf8 version " << MAJOR << "." << MINOR << "." << PATCH << std::endl;
        throw EarlyFinish();
    }
    if (bits[0]) {
        if (bits[2]) {
            std::cout << helpmsg; // helpmsg already has a newline at the end
            throw EarlyFinish();
        } else {
            std::cerr << "The given input is not a valid usage of this program.\nUse 'escape --help' for usage information." << std::endl;
            throw InvalidCmd();
        }
    }
    assert(bits[2]);
    // The only remaining case is the one where we can continue with the rest of the program.
    // Before setting up the streams, we do some setup on stdin/stdout. We do this here
    // in order to make the modifications before creating StreamPair, but not if any
    // help/version message is printed.
#ifdef _WIN32
    /* stdin and stdout are opened in text mode. On Windows, that means they
     * do unwanted line-ending translation. To fix that we need to set stdin
     * and stdout to be binary streams instead.
     * We use _setmode to do that:
     * https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setmode
     * https://stackoverflow.com/a/11259588
     */
    int result_in = _setmode(_fileno(stdin), _O_BINARY);
    int result_out = _setmode(_fileno(stdout), _O_BINARY);
    if (result_in == -1 || result_out == -1) {
        std::cerr << "Error when setting stdin/stdout to binary mode. Exiting now." << std::endl;
        throw WindowsIOError();
    }
#endif

    // This line should improve I/O performance. But it makes this program not thread-safe.
    // We're not using multi-threading, so that's all right.
    std::ios_base::sync_with_stdio(false);

    if (inputfile.empty()) {
        if (outputfile.empty()) {
            return StreamPair(true, true);
        } else {
            return StreamPair(true, outputfile);
        }
    } else {
        if (outputfile.empty()) {
            return StreamPair(inputfile, true);
        } else {
            return StreamPair(inputfile, outputfile);
        }
    }
}

/**
 * This is a helper function that parses the command-line args and returns a
 * representation of the given arguments.
 * @param argc The argc from main()
 * @param argv The argv from main()
 * @param inputfile This is a return value, passed by reference into parse_helper(). If the
 * command-line args specify INPUTFILE then it will be stored in this string; otherwise
 * this string will not be modified.
 * This string must be empty when passed into parse_helper() so that you will be able to tell
 * whether something has been stored in it.
 * @param outputfile Just like inputfile, but for the OUTPUTFILE arg.
 * @return A bitset with 3 bits. This encodes the relevant information about the given args.
 * Bit 0 is the "help" bit; it is 1 iff you should print a help message and exit.
 * Bit 1 is the "version" bit; it is 1 iff you should print the version-number message and exit.
 * Bit 2 is the "valid" bit. It is 1 if the given command-line args are valid and you
 * can continue with the program; it is 0 if the args are invalid in some way, in which case
 * you should print an "invalid args" message and exit.
 *
 * If the "valid" bit is 0 then it is guaranteed that the "help" bit will be 1.
 * If the "version" bit is 1 then it is guaranteed that the "valid" bit will be 1.
 * Note that even if the "valid" bit is 1, you may still have to end the program soon,
 * either because of the help flag or the version flag.
 */
std::bitset<3> parse_helper(int argc, char **argv, std::string& inputfile, std::string& outputfile) {
    /*
     * Defined these as std::strings so that I could easily do something like
     * argv[1] == dash_o` and get the overloaded operator== that compares
     * char* to std::string.
     */
    std::string const dash_o("-o");
    std::string const dash_dash_output("--output");
    std::string const dash_h("-h");
    std::string const dash_dash_help("--help");
    std::string const dash_v("-v");
    std::string const dash_dash_version("--version");


    assert(inputfile.empty());
    assert(outputfile.empty());
    std::bitset<3> bits;
    assert(!bits.any()); // By default, all bits are set to 0
    if (argc > 4) {
        // 4 is the highest that argc can ever be for a valid invocation.
        // So here we return "help" and "invalid".
        bits.set(0);
        return bits;
    }
    if (argc == 4) {
        // The 3 given args must be INPUTFILE, -o/--output, and OUTPUTFILE, else it's invalid
        if (argv[1] == dash_o || argv[1] == dash_dash_output) {
            outputfile.assign(argv[2]);
            inputfile.assign(argv[3]);
        } else if (argv[2] == dash_o || argv[2] == dash_dash_output) {
            inputfile.assign(argv[1]);
            outputfile.assign(argv[3]);
        } else {
            bits.set(0); // Return "help" and "invalid"
            return bits;
        }
        bits.set(2); // Return "valid"
        return bits;
    } else if (argc == 3) {
        // There are two valid possibilities: either just -o/--output OUTPUTFILE was given,
        // or both INPUTFILE and OUTPUTFILE were given but using the -oOUTPUTFILE
        // or --output=OUTPUTFILE syntax.
        int i = check_output_option(argv[1]);
        if (i >= 2) { // This means argv[1] has the OUTPUTFILE option and argv[2] is INPUTFILE.
            assert(i==2 || i==9);
            outputfile.assign(argv[1] + i); // In this case, i marks the start of the actual arg
            inputfile.assign(argv[2]);

            bits.set(2);
            return bits;
        } else if (i == 1) {
            // This means argv[1] is "-o" or "--output" and argv[2] should be interpreted as OUTPUTFILE.
            outputfile.assign(argv[2]);
            // We don't assign inputfile, to denote it wasn't specified.
            bits.set(2);
            return bits;
        }
        // Otherwise we couldn't parse the first arg so we'll see if
        // the second arg is the OUTPUTFILE option.
        // Note: if the first arg was "--output=" and the second arg turns out to have a valid output
        // argument (such as "-ofilename") then "--output=" will be treated as the input filename.
        int j = check_output_option(argv[2]);
        if (j >= 2) { // This means argv[2] has the OUTPUTFILE option and argv[1] is INPUTFILE.
            assert(j==2 || j==9);
            inputfile.assign(argv[1]);
            outputfile.assign(argv[2] + j);

            bits.set(2);
            return bits;
        }
        // Otherwise the input is not well-formed.
        bits.set(0);
        return bits;
    } else if (argc == 2) {
        // There's a single arg. The valid options are: help flag, version flag,
        // INPUTFILE, or combined option flag and OPTIONFILE.
        if (argv[1] == dash_h || argv[1] == dash_dash_help) {
            bits.set(0);
            bits.set(2); // Passing just the help flag is a valid command.
            return bits;
        } else if (argv[1] == dash_v || argv[1] == dash_dash_version) {
            bits.set(1);
            bits.set(2);
            return bits;
        }
        int i = check_output_option(argv[1]);
        if (i >= 2) { // This means the arg is the combined option flag and OPTIONFILE.
            assert(i==2 || i==9);
            outputfile.assign(argv[1] + i);

            bits.set(2);
            return bits;
        }
        // Note: for an output option with no corresponding argument, we treat the option itself as the input filename
        inputfile.assign(argv[1]);
        bits.set(2);
        return bits;
    } else { // This is the case with zero arguments. Pretty easy to handle.
        assert(argc == 1);
        bits.set(2);
        return bits;
    }
}

/**
 * Checks whether the given string begins with either "-o" or "--output=",
 * and whether it contains at least 1 character beyond that.
 * @param arg Null-terminated string
 * @return If BOTH of the conditions above hold, returns the index of the
 * first character beyond the prefix. This will either be 2 (for a prefix of "-o"),
 * or 9 (for a prefix of "--output=").
 * Otherwise:
 *   If arg is "-o" or "--output", returns 1.
 *   Else if arg is "--output=", returns 0.
 *   Else, returns -1.
 */
int check_output_option(const char *arg) {
    if (!std::strncmp(arg, "-o", 2)) {
        return strlen_atleast(arg, 3) ? 2 : 1;
    } else if (!std::strncmp(arg, "--output=", 9)) {
        return strlen_atleast(arg, 10) ? 9 : 0;
    } else if (arg == std::string("--output")) {
        return 1;
    } else {
        return -1;
    }
}

/**
 * Returns whether the given null-terminated string has a length of at least len,
 * not including the null byte.
 *
 * This function will not read more than len characters from the string, so if
 * the string's length is much larger than len, this function will be faster than
 * the expression (strlen(str) >= len).
 * @param str A null-terminated string.
 * @param len The desired length to check for.
 * @return True if there are at least len non-null characters in str, false otherwise.
 */
bool strlen_atleast(const char *str, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) {
        if (str[i] == '\0') {
            return false;
        }
    }
    return true;
}
