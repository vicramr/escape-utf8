import java.io.IOException;

class BusinessLogic {

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
            System.err.println("Failed to open output file \"" + outputfile + "\". Exiting now.");
            return 1;
        }
        // TODO: use the streams to read and escape
        return 0;
    }
}
