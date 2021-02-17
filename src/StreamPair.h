//
// Created by Vicram on 8/31/2019.
//

#ifndef ESCAPE_UTF8_STREAMPAIR_H
#define ESCAPE_UTF8_STREAMPAIR_H

#include <istream>
#include <ostream>
#include <exception>
#include <string>
#include <memory>

class FileError : public std::exception {};

/**
 * This is a class that encapsulates the input & output streams
 * used throughout the program.
 *
 * Usage:
 *   All resources are managed internally. The user doesn't need to worry about
 *   deallocating anything.
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
 *   I'm using shared_ptrs to hold the streams largely because unique_ptr isn't
 *   copy-constructible or copy-assignable. I need to be able to construct the
 *   StreamPair in one function and then pass it to another function. The best
 *   solution would probably be to do a move, but in all honesty I don't quite
 *   trust myself to write correct code with moves. 
 *   This also rules out storing the value of the object itself, as opposed to a pointer.
 *   That wouldn't work because copy assignment is not allowed for streams; it doesn't 
 *   make sense to copy them. It also wouldn't work because I want polymorphism; I need to
 *   be able to store a pointer to the parent class (std::istream and std::ostream).
 *
 *
 *   There's one other issue: std::istream and std::ostream use signed char in the
 *   type parameter by default. This is annoying, because I use unsigned chars everywhere,
 *   but it's necessary because std::cin and std::cout are parameterized on signed chars.
 *   This just means we have to cast in business_logic.cpp.
 */
class StreamPair {
public:
    std::shared_ptr<std::istream> in;
    std::shared_ptr<std::ostream> out;
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
private:
    void check_in(const std::string& inputfile);
    void check_out(const std::string& outputfile);
};


#endif //ESCAPE_UTF8_STREAMPAIR_H
