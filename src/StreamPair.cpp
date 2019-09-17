//
// Created by Vicram on 8/31/2019.
//

#include <fstream>
#include <iostream>

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
    in(new std::ifstream(inputfile)), 
    out(new std::ofstream(outputfile)) {
        check_in(inputfile);
        check_out(outputfile);
}

StreamPair::StreamPair(const std::string &inputfile, bool) : 
    in(new std::ifstream(inputfile)), 
    out(&std::cout, deleter) {
        check_in(inputfile);
}

StreamPair::StreamPair(bool, const std::string &outputfile) :
    in(&std::cin, deleter),
    out(new std::ofstream(outputfile)) {
        check_out(outputfile);
}

StreamPair::StreamPair(bool, bool) :
    in(&std::cin, deleter),
    out(&std::cout, deleter) {}
