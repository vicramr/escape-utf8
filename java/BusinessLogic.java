import java.io.IOException;
import java.util.Locale;

class BusinessLogic {

    private static final String MALFORMED = "The given text is not valid UTF-8 text. Exiting now.";

    /**
     * This is the core method of the program. This method will read input,
     * escape it, and write the escaped output.
     * 
     * inputfile: This may either be a string or null. If null, this method
     *     will use stdin for input. Otherwise inputfile will be used as the
     *     name of the input file.
     * outputfile: Same as inputfile, but used for the output file. In particular,
     *     if this is null, output will be written to stdout.
     *
     * return: the exit code for the program. The caller should call System.exit
           using this value.
     */
    static int run(String inputfile, String outputfile) {
        StreamPair streams;
        try {
            streams = new StreamPair(inputfile, outputfile);
        } catch (StreamPairException e) {
            System.err.println(e.getMessage());
            return 1;
        } catch (IOException e) {
            // See StreamPair docstring for info on when this can happen.
            System.err.println("Failed to open output file \"" + outputfile + "\". Also encountered error when closing input file. Exiting now.");
            return 1;
        }

        int exitcode = run_impl(streams);
        try {
            streams.closeStreams();
        } catch (IOException e) {
            System.err.println("Error when closing input and/or output file.");
            return 5;
        }
        return exitcode;
    }

    /**
     * This method contains most of the implementation for the run method.
     * Given a newly-created StreamPair, this method will do the reading,
     * parsing, and writing. This will also handle any errors and print
     * any error messages.
     *
     * NOTE: this method will NOT close the streams.
     *
     * The return value is the exit code for the program.
     */
    private static int run_impl(StreamPair streams) {
        while (true) {
            assert !streams.out.checkError();

            int octet;
            try {
                octet = streams.in.read();
            } catch (IOException e) {
                System.err.println("Failed when trying to read from input due to unknown error.");
                return 3;
            }
            if (octet == -1) {
                // This is the success case: we have reached EOF and we weren't
                // in the middle of a character.
                return 0;
            }
            assert 0 <= octet && octet <= 255;

            if (octet <= 127) { // One-byte character
                // First case: printable characters
                if ((32 <= octet && octet <= 126) || octet == 9 || octet == 10 || octet == 13) {
                    streams.out.write(octet);
                } else { // Second case: non-printable chars
                    assert octet == 127 || octet <= 31;
                    streams.out.format(Locale.US, "\\u'%04X'", octet);
                }
            } else { // 2+ byte character
                int numbytes = getLen(octet);
                if (numbytes == -1) {
                    System.err.println(MALFORMED);
                    return 2;
                }
                int mask = getMask(numbytes);

                // First we get the lower-order bits into position in decoded_char,
                // then we read the next few bytes and construct the codepoint in decoded_char.
                int decoded_char = octet & mask;
                for (int i = 1; i < numbytes; ++i) {
                    try {
                        octet = streams.in.read();
                    } catch (IOException e) {
                        System.err.println("Failed when trying to read from input due to unknown error.");
                        return 3;
                    }
                    if (octet == -1) { // EOF in the middle of a multibyte character
                        System.err.println(MALFORMED);
                        return 2;
                    }
                    assert 0 <= octet && octet <= 255;

                    // Now that we have successfully read a byte, we must check whether
                    // it is valid and, if so, extract the lower 6 bits from it.
                    if ((octet & 0b11000000) != 0b10000000) { // Invalid case
                        System.err.println(MALFORMED);
                        return 2;
                    }
                    // Otherwise the byte is valid.
                    decoded_char <<= 6;
                    decoded_char |= (octet & 0b00111111);
                }
                // Now decoded_char holds the value of the decoded codepoint. We still need to check
                // that this is in the valid range of codepoints, given the number of bytes.
                if (numbytes == 2 && (decoded_char < 0x80 || decoded_char > 0x7FF)) {
                    System.err.println(MALFORMED);
                    return 2;
                }
                if (numbytes == 3 && (decoded_char < 0x800 || decoded_char > 0xFFFF)) {
                    System.err.println(MALFORMED);
                    return 2;
                }
                if (numbytes == 4 && (decoded_char < 0x10000 || decoded_char > 0x10FFFF)) {
                    System.err.println(MALFORMED);
                    return 2;
                }
                // We have finished checking the value of the character so now we can print it.
                streams.out.format(Locale.US, "\\u'%04X'", decoded_char);
            }
        // At the end of each iter of the while loop, if we reach this point, then it is
        // guaranteed that we have written to streams.out exactly once (with either write or format).
        // Therefore we must check the stream at this point.
        if (streams.out.checkError()) {
            System.err.println("There was a fatal error when trying to write to the output. Exiting now.");
            return 4;
        }
        // Otherwise both streams are fine and we're done with this character, so we can simply move on to the next iter.
        }
    }

    /**
     * Given the first byte of a multibyte UTF-8 character, this method returns the
     * total length of the character: either 2, 3, or 4. If the given byte is not
     * a valid first byte of a multibyte UTF-8 character then this method returns -1.
     */
    private static int getLen(int octet) {
        assert 0 <= octet && octet <= 255;
        if ((octet & 0b11100000) == 0b11000000) {
            return 2;
        } else if ((octet & 0b11110000) == 0b11100000) {
            return 3;
        } else if ((octet & 0b11111000) == 0b11110000) {
            return 4;
        } else {
            return -1;
        }
    }

    /**
     * This method returns the mask used to select the low-order bits from
     * the first byte in a multibyte character.
     */
     private static int getMask(int numbytes) {
         if (numbytes == 2) {
             return 0b00011111;
         } else if (numbytes == 3) {
             return 0b00001111;
         } else {
             assert numbytes == 4;
             return 0b00000111;
         }
     }
}
