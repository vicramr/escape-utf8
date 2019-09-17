//
// Created by Vicram on 8/22/2019.
//
#include <iostream>

#include "parseargs.h"
#include "StreamPair.h"
#include "business_logic.h"

int main(int argc, char *argv[]) {
    // This line should improve I/O performance. But it makes this program not thread-safe.
    // We're not using multi-threading, so that's all right.
    std::ios_base::sync_with_stdio(false);


    try {
        StreamPair streams = parse(argc, argv);
        int retval = read_and_escape(streams);
        return retval;
    } catch (const EarlyFinish& e) {
        return 0;
    } catch (const FileError& e) {
        return 1;
    }
}
