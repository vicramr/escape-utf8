//
// Created by Vicram on 8/22/2019.
//
#include <iostream>

#include "parseargs.h"
#include "StreamPair.h"
#include "business_logic.h"

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


/*
 * EXIT CODES
 * 0: success
 * 1: failed to open input/output file
 * 2: the given text is not valid UTF-8 (e.g. the file ended in the middle of
 *    a multi-byte character)
 * 3: error when trying to read from input file
 * 4: error when trying to write to output file
 * 5: malformed command line
 * 6: error when setting stdin/stdout to binary mode (Windows only)
 */

int main(int argc, char *argv[]) {

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
        std::cerr << "Error when setting stdin and stdout to binary mode. Exiting now." << std::endl;
        return 6;
    }
#endif

    // This line should improve I/O performance. But it makes this program not thread-safe.
    // We're not using multi-threading, so that's all right.
    std::ios_base::sync_with_stdio(false);


    try {
        StreamPair streams = parse(argc, argv);
        int retval = read_and_escape(streams);
        return retval;
    } catch (const EarlyFinish&) {
        return 0;
    } catch (const FileError&) {
        return 1;
    } catch (const InvalidCmd&) {
        return 5;
    }
}
