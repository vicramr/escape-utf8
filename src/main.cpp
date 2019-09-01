//
// Created by Vicram on 8/22/2019.
//
#include <iostream>

#include "parseargs.h"
#include "StreamPair.h"

int main(int argc, char *argv[]) {
    try {
        StreamPair streams = parse(argc, argv);
    } catch (EarlyFinish& e) {
        return 0;
    } catch (FileError& e) {
        return 1;
    }
    std::cout << "Parsing command-line args succeeded!" << std::endl;
    return 0;
}