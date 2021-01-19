/**
 * This is the class with the main method. This file contains the logic
 * to parse the command-line arguments and give the results to the rest of
 * the program.
 */

import java.util.List;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.ParseException;

class Escape {

    private static final String versionStr = "0.0.2";

    private static final String linesep = System.lineSeparator(); // https://stackoverflow.com/a/30423625
    private static final String usage = linesep +
                                        "  escape [INPUTFILE] [-o OUTPUTFILE]" + linesep +
                                        "  escape -h | --help" + linesep +
                                        "  escape -v | --version" + linesep + linesep +
                                        "argument:" + linesep +
                                        "  INPUTFILE: Path to the input file. If omitted, input will be" + linesep +
                                        "             read from stdin." + linesep + linesep +
                                        "options:" + linesep;

    public static void main(String[] args) {
        // Command-line parsing code is modified from here: https://stackoverflow.com/a/367714
        Options options = new Options();

        Option output = new Option("o", "output", true, "The output filename. If omitted, the output will be printed to stdout.");
        options.addOption(output);

        Option help = new Option("h", "help", false, "Print this help message.");
        options.addOption(help);

        Option version = new Option("v", "version", false, "Print version information.");
        options.addOption(version);

        CommandLineParser parser = new DefaultParser();
        HelpFormatter formatter = new HelpFormatter();
        formatter.setArgName("OUTPUTFILE");

        CommandLine cmd = null;

        try {
            cmd = parser.parse(options, args);

            if (cmd.hasOption("help")) {
                System.out.println("escape-utf8: Transform UTF-8 text to a representation in ASCII.");
                System.out.println("For detailed information please see the README.");
                System.out.println("");
                formatter.printHelp(usage, options);
                System.exit(0);
            }
            if (cmd.hasOption("version")) {
                System.out.println("escape-utf8 version " + versionStr);
                System.exit(0);
            }

            List<String> nonoptions = cmd.getArgList();
            if (nonoptions.size() > 1) {
                throw new ParseException("Too many arguments");
            }
        } catch (ParseException e) {
            System.err.println(e.getMessage());
            System.err.println("Use the --help option to see usage instructions.");
            System.exit(5);
        }

        String inputfile;
        String outputfile;

        List<String> nonoptions = cmd.getArgList();
        if (nonoptions.size() == 1) {
            inputfile = nonoptions.get(0);
        } else {
            assert nonoptions.size() == 0;
            inputfile = null;
        }

        if (cmd.hasOption("output")) {
            outputfile = cmd.getOptionValue("output");
        } else {
            outputfile = null;
        }

        int exitcode = BusinessLogic.run(inputfile, outputfile);
        System.exit(exitcode);
    }
}
