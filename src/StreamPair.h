//
// Created by Vicram on 8/31/2019.
//

#ifndef ESCAPE_UTF8_STREAMPAIR_H
#define ESCAPE_UTF8_STREAMPAIR_H

#include <istream>
#include <ostream>

/**
 * This is a class that encapsulates the input & output streams
 * used throughout the program.
 *
 * I wanted to carry around smart pointers to the streams rather
 * than raw pointers, but I think this is fundamentally impossible:
 *   - in must be able to point to either a std::ifstream or std::cin, and
 *     out must be able to point to either a std::ofstream or std::cout.
 *   - It doesn't make sense to have any kind of smart pointer to std::cin
 *     or std::cout, because I'm not managing the lifetime of those objects.
 * Barring that, I would've liked to store the value of the object itself
 * in this class, but that won't work either because copy assignment is
 * not allowed for streams; it doesn't make sense to copy them.
 *
 * So I have to resort to using raw pointers. This can be dangerous.
 * The main guideline to follow is this: when constructing instances
 * of this class, each input should either be a pointer to std::cin/std::cout,
 * OR a pointer to an object constructed using 'new'.
 */
class StreamPair {
public:
    std::istream *in;
    std::ostream *out;
    StreamPair() = delete;

    StreamPair(const std::string& inputfile, const std::string& outputfile);
    StreamPair(const std::string& inputfile, bool b);
    StreamPair(bool a, const std::string& outputfile);
    StreamPair(bool a, bool b);
private:
    bool delete_in;
    bool delete_out;
};


#endif //ESCAPE_UTF8_STREAMPAIR_H
