//
// Created by Vicram on 8/31/2019.
//

#include <fstream>
#include <iostream>
#include <ios> // for ios_base::binary

#include "StreamPair.h"

/*
 * IMPLEMENTATION NOTES
 * Shared Pointer Constructors:
 *   For the cases where the filename is given, I'm using shared_ptr constructor 3:
 *   https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
 *   This will do 'delete ptr' on the object, which is the desired behavior.
 * 
 *   For the cases where no filename is given, we don't want to do 'delete ptr'
 *   because std::cin and std::cout are global variables. So I instead use constructor 4
 *   and pass in a dummy deleter which doesn't do anything. I think that the way I created
 *   and passed the deleter is valid, because one of the examples in that ref page declares
 *   the deleter like this:
 *     [](auto p) {std::cout << "Call delete from lambda...\n"; delete p;}
 * 
 * Error Handling:
 *   This reference page documents all the ways that I/O functions can fail:
 *   https://en.cppreference.com/w/cpp/io/ios_base/iostate
 *   It looks like constructing ifstream/ofstream can only fail if the file cannot
 *   be opened. If that happens, then the failbit is set; see here (we're using constructor 4):
 *   https://en.cppreference.com/w/cpp/io/basic_ifstream/basic_ifstream
 *
 * Binary Mode:
 *   By default, it seems that ifstream/ofstream are opened in text mode. Details for
 *   text mode are here: https://en.cppreference.com/w/cpp/io/c/FILE#Binary_and_text_modes
 *   This doesn't matter for POSIX systems but it does for Windows. On Windows, CRLF
 *   will be converted to LF on input and vice versa for output.
 *   The docs for the constructors for basic_ifstream and basic_ofstream discuss how
 *   to add the mode arg. There is also some relevant documentation from Microsoft:
 *   https://docs.microsoft.com/en-us/cpp/standard-library/binary-output-files?view=msvc-160
 *
 *   NOTE: std::cin and std::cout are opened in text mode! And according to SO, there
 *   is not a platform-independent way to change that: https://stackoverflow.com/a/7587701
 *   In other words, on Windows this program does not function 100% correctly unless
 *   the user gives a filename for both input and output.
 */

auto deleter = [](auto){}; // parameter name omitted to silence "unused parameter" warnings

void StreamPair::check_in(const std::string& inputfile) {
    if (in->fail()) {
        std::cerr << "Failed to open input file \"" << inputfile << "\". Exiting now." << std::endl;
        throw FileError();
    }
}

void StreamPair::check_out(const std::string &outputfile) {
    if (out->fail()) {
        std::cerr << "Failed to open output file \"" << outputfile << "\". Exiting now." << std::endl;
        throw FileError();
    }
}

StreamPair::StreamPair(const std::string &inputfile, const std::string &outputfile) : 
    in(new std::ifstream(inputfile, std::ios_base::binary)),
    out(new std::ofstream(outputfile, std::ios_base::binary)) {
        check_in(inputfile);
        check_out(outputfile);
}

StreamPair::StreamPair(const std::string &inputfile, bool) : 
    in(new std::ifstream(inputfile, std::ios_base::binary)),
    out(&std::cout, deleter) {
        check_in(inputfile);
}

StreamPair::StreamPair(bool, const std::string &outputfile) :
    in(&std::cin, deleter),
    out(new std::ofstream(outputfile, std::ios_base::binary)) {
        check_out(outputfile);
}

StreamPair::StreamPair(bool, bool) :
    in(&std::cin, deleter),
    out(&std::cout, deleter) {}
