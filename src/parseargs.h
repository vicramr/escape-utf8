//
// Created by Vicram on 8/27/2019.
//

#ifndef ESCAPE_UTF8_PARSEARGS_H
#define ESCAPE_UTF8_PARSEARGS_H

#include <istream>
#include <ostream>
#include <memory>
#include <utility>

/**
 * This is a high-level function that parses command-line arguments, checks
 * that the arguments are well-formed, does error handling, and opens any
 * needed files for reading/writing.
 * If the command-line args are not well-formed, or the user passes the
 * -h/--help or -v/--version options, or there is any error, then this
 * function will print any necessary help messages and error messages and
 * exit the program. Otherwise, this function will succeed and return the
 * input and output streams to be used in the rest of the program.
 * @param argc The argc value from main().
 * @param argv The argv value from main().
 * @return A std::istream and a std::ostream. The istream might be from
 * stdin or from a file; the ostream might be for stdout or for a file.
 * The details of where the streams point to are not relevant for the
 * caller; the caller should just treat these as streams of bytes.
 * As mentioned above, this function is not guaranteed to return.
 */
std::pair<std::unique_ptr<std::istream>, std::unique_ptr<std::ostream>>
parse(int argc, char **argv);

#endif //ESCAPE_UTF8_PARSEARGS_H
