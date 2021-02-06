//
// Created by Vicram on 8/22/2019.
//
#include <iostream>

#include "parseargs.h"
#include "StreamPair.h"
#include "business_logic.h"


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
    } catch (const WindowsIOError&) {
        return 6;
    }
}
