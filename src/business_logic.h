//
// Created by Vicram on 9/3/2019.
//

#ifndef ESCAPE_UTF8_BUSINESS_LOGIC_H
#define ESCAPE_UTF8_BUSINESS_LOGIC_H

#include "StreamPair.h"

/**
 * This is the central function of the whole program. This function reads the
 * input, escapes any non-ASCII characters and writes out the escaped output.
 * If there's any error or the input text is malformed, this function will
 * write out an error message to stderr and return a nonzero value. Otherwise
 * it will return 0.
 * @param streams A StreamPair
 * @return int which should be used as the exit status for the whole program.
 */
int read_and_escape(const StreamPair& streams);

#endif //ESCAPE_UTF8_BUSINESS_LOGIC_H
