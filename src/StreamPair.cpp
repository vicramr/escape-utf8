//
// Created by Vicram on 8/31/2019.
//

#include <fstream>
#include <iostream>

#include "StreamPair.h"

/*
 * I've made init_in and init_out standalone functions so that I can encapsulate the
 * error handling.
 * This reference page documents all the ways that I/O functions can fail:
 * https://en.cppreference.com/w/cpp/io/ios_base/iostate
 *
 * It looks like constructing ifstream/ofstream can only fail if the file cannot
 * be opened. If that happens, then the failbit is set; see here (we're using constructor 4):
 * https://en.cppreference.com/w/cpp/io/basic_ifstream/basic_ifstream
 *
 * Also note that if there's an exception thrown in StreamPair's constructor, StreamPair's
 * destructor will not be called, so we still need to clean up any allocated fstreams.
 * See FAQ: https://isocpp.org/wiki/faq/exceptions#selfcleaning-members
 * (The FAQ recommends dealing with the issue by using smart pointers, but we're using raw pointers
 * so we have to manually delete.)
 */

void StreamPair::init_in(const std::string &inputfile) {
    in = new std::ifstream(inputfile);
    delete_in = true;

    if (in->fail()) {
        std::cout << "Failed to open input file \"" << inputfile << "\". Exiting now." << std::endl;
        delete in;
        throw FileError();
    }
}

void StreamPair::init_out(const std::string &outputfile) {
    out = new std::ofstream(outputfile);
    delete_out = true;

    if (out->fail()) {
        std::cout << "Failed to open output file \"" << outputfile << "\". Exiting now." << std::endl;
        // init_out is always called after init_in so we need to worry about deallocating in here
        if (delete_in) {
            delete in;
        }
        delete out;
        throw FileError();
    }
}

StreamPair::StreamPair(const std::string &inputfile, const std::string &outputfile) {
    init_in(inputfile);

    init_out(outputfile);
}

StreamPair::StreamPair(const std::string &inputfile, bool) {
    init_in(inputfile);

    out = &std::cout;
    delete_out = false;
}

StreamPair::StreamPair(bool, const std::string &outputfile) {
    in = &std::cin;
    delete_in = false;

    init_out(outputfile);
}

StreamPair::StreamPair(bool, bool) {
    in = &std::cin;
    delete_in = false;

    out = &std::cout;
    delete_out = false;
}

StreamPair::~StreamPair() {
    if (delete_in) {
        delete in;
    }
    if (delete_out) {
        delete out;
    }
}