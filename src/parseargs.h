//
// Created by Vicram on 8/27/2019.
//

#ifndef ESCAPE_UTF8_PARSEARGS_H
#define ESCAPE_UTF8_PARSEARGS_H


#include <memory>
#include <exception>

#include "StreamPair.h"

class EarlyFinish : public std::exception {};

/**
 * This is a high-level function that parses command-line arguments, checks
 * that the arguments are well-formed, does error handling, and opens any
 * needed files for reading/writing.
 *
 * If the command-line args are not well-formed, or the user passes the
 * -h/--help or -v/--version options, then this function will print any necessary
 * help messages and throw an EarlyFinish exception.
 * If there's an error when trying to open a file for reading/writing, then this
 * function will throw a FileError exception.
 * Otherwise, this function will succeed and return the input and output streams
 * to be used in the rest of the program.
 * @param argc The argc value from main().
 * @param argv The argv value from main().
 * @return A pair of a std::istream and a std::ostream. The istream might be from
 * stdin or from a file; the ostream might be for stdout or for a file.
 * The details of where the streams point to are not relevant for the
 * caller; the caller should just treat these as streams of bytes.
 * @throws As discussed above, this function can throw an EarlyFinish exception or a
 * FileError exception.
 *
 * For EarlyFinish the caller should clean up and exit the program with exit status 0;
 * this is not considered an error.
 * For FileError the caller should clean up and exit the program with nonzero exit status;
 * this is considered an error.
 */
StreamPair parse(int argc, char **argv);

#endif //ESCAPE_UTF8_PARSEARGS_H
