//
// Created by Vicram on 8/31/2019.
//

#ifndef ESCAPE_UTF8_STREAMPAIR_H
#define ESCAPE_UTF8_STREAMPAIR_H

#include <istream>
#include <ostream>
#include <exception>

class FileError : public std::exception {};

/**
 * This is a class that encapsulates the input & output streams
 * used throughout the program.
 *
 * Usage:
 *   The user must call free() when done with this object in order to make sure
 *   that all allocated resources are deallocated.
 *
 *   There are 4 constructors, and all are used the same way. The first positional
 *   argument is the name of the input file or, if we're reading from stdin, a
 *   bool. The second positional argument is analogous for the output file.
 *   For the bools, it doesn't matter whether you pass true or false; the parameter is just a
 *   tag to differentiate the cases where you're either only passing the inputfile
 *   or only passing the outputfile.
 *
 *   THE CONSTRUCTORS CAN THROW. Well, the constructors taking at least one string
 *   as input can throw. If a file cannot be opened for reading/writing, then the
 *   constructor will print an error message and throw a FileError exception. In this
 *   case, there's no use continuing with the program, so it's a signal to clean up and
 *   end the program.
 *
 * Implementation notes:
 *   See the C++ FAQ here for why throwing exceptions in the constructor is the
 *   best practice: https://isocpp.org/wiki/faq/exceptions#ctors-can-throw
 *
 *
 *   I wanted to carry around smart pointers to the streams rather
 *   than raw pointers, but I think this is fundamentally impossible:
 *     - in must be able to point to either a std::ifstream or std::cin, and
 *       out must be able to point to either a std::ofstream or std::cout.
 *     - It doesn't make sense to have any kind of smart pointer to std::cin
 *       or std::cout, because I'm not managing the lifetime of those objects.
 *   Barring that, I would've liked to store the value of the object itself
 *   in this class, but that won't work either because copy assignment is
 *   not allowed for streams; it doesn't make sense to copy them. It also
 *   wouldn't work because I want covariance; I want to be able to store a pointer
 *   to the parent class (std::istream and std::ostream).
 *   So I have to resort to using raw pointers.
 */
class StreamPair {
public:
    std::istream *in;
    std::ostream *out;
    StreamPair() = delete;

    /*
     * In these constructors, the bool parameter serves only as a tag to
     * differentiate between different constructors that would otherwise
     * have the same signature; that is, the constructors that take 1 string.
     * It is not used, so I have omitted the parameter name.
     */
    StreamPair(const std::string& inputfile, const std::string& outputfile);
    StreamPair(const std::string& inputfile, bool);
    StreamPair(bool, const std::string& outputfile);
    StreamPair(bool, bool);
    void free();
private:
    bool delete_in;
    bool delete_out;

    void init_in(const std::string &inputfile);
    void init_out(const std::string &outputfile);
};


#endif //ESCAPE_UTF8_STREAMPAIR_H
