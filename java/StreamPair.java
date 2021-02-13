import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.Files;
import java.nio.file.InvalidPathException;
import java.nio.charset.StandardCharsets;

/**
 * This class encapsulates the input and output streams.
 * Once you are finished with both streams, you should call closeStreams.
 */
class StreamPair {
    boolean usingStdin;
    InputStream in;
    boolean usingStdout;
    PrintStream out;

    /**
     * Given the input filename and output filename, this constructor will open
     * two streams and set the in and out members accordingly.
     * If either inputfile or outputfile is null, then stdin/stdout will be used
     * for the stream, respectively.
     * If we fail to open either file then this constructor will throw a
     * StreamPairException with an error message. The caller should print this
     * message to stderr before exiting.
     *
     * If this constructor throws an exception, you do not need to call closeStreams.
     *
     * Note: this constructor should only throw an IOException in the case that
     * we failed to open the output file, AND we had already opened an input file,
     * AND the input file threw an IOException upon close().
     */
    StreamPair(String inputfile, String outputfile) throws StreamPairException, IOException {
        if (inputfile == null) {
            usingStdin = true;
            in = System.in;
        } else {
            usingStdin = false;

            InputStream instream = null;
            try {
                Path path = Paths.get(inputfile);
                instream = Files.newInputStream(path);
            } catch (InvalidPathException | IOException | SecurityException e) {
                assert instream == null; // TODO is it possible for this to be non-null here?
                // We could print out a more detailed error message, but this is identical
                // to the message printed by the C++ implementation.
                throw new StreamPairException("Failed to open input file \"" + inputfile + "\". Exiting now.");
            }
            in = new BufferedInputStream(instream);
        }

        if (outputfile == null) {
            usingStdout = true;
            out = System.out;
        } else {
            usingStdin = false;

            OutputStream outstream = null;
            try {
                Path path = Paths.get(outputfile);
                outstream = Files.newOutputStream(path);
            } catch (InvalidPathException | IOException | SecurityException e) {
                assert outstream == null;
                // Because we may have opened an input file, we must also close it.
                if (!usingStdin) {
                    in.close();
                }
                throw new StreamPairException("Failed to open output file \"" + outputfile + "\". Exiting now.");
            }
            try {
                out = new PrintStream(outstream, false, StandardCharsets.US_ASCII.name());
            } catch (UnsupportedEncodingException e) {
                assert false; // US_ASCII is guaranteed to be supported by any implementation of the JVM.
            }
        }
    }

    /**
     * This method closes any streams associated with files.
     */
    void closeStreams() throws IOException {
        try {
            if (!usingStdin) {
                in.close();
            }
        } finally {
            if (!usingStdout) {
                out.close();
            }
        }
    }
}
