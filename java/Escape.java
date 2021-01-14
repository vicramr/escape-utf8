import java.util.List;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.ParseException;

class Escape {

    private static final String versionStr = "0.0.0";

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
            System.out.println(e.getMessage());
            formatter.printHelp(usage, options);

            System.exit(1);
        }
        
        String inputfile;
        String outputfile;
        
        List<String> nonoptions = cmd.getArgList();
        if (nonoptions.size() == 1) {
            inputfile = nonoptions.get(0);
        } else {
            assert nonoptions.size() == 0;
            inputfile = "no input file given";
        }

        if (cmd.hasOption("output")) {
            outputfile = cmd.getOptionValue("output");
        } else {
            outputfile = "no output file given";
        }

        System.out.println("Input file: " + inputfile);
        System.out.println("Output file: " + outputfile);
    }
}
