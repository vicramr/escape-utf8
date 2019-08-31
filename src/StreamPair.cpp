//
// Created by Vicram on 8/31/2019.
//

#include <fstream>
#include <iostream>

#include "StreamPair.h"




StreamPair::StreamPair(const std::string &inputfile, const std::string &outputfile) {
    in = new std::ifstream(inputfile);
    delete_in = true;

    out = new std::ofstream(outputfile);
    delete_out = true;
}

StreamPair::StreamPair(const std::string &inputfile, bool b) {
    in = new std::ifstream(inputfile);
    delete_in = true;

    out = &std::cout;
    delete_out = false;
}

StreamPair::StreamPair(bool a, const std::string &outputfile) {
    in = &std::cin;
    delete_in = false;

    out = new std::ofstream(outputfile);
    delete_out = true;
}

StreamPair::StreamPair(bool a, bool b) {
    in = &std::cin;
    delete_in = false;

    out = &std::cout;
    delete_out = false;
}
