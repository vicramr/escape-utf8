This directory contains a Java implementation of the escape program. This code is written for Java 8 and I cannot guarantee that it will work for any other version of Java. This implementation has the exact same command-line interface as the C++ implementation, and the exit codes are also mostly identical. A few error messages are slightly different, but other than that they should be functionally equivalent.

This project uses the Apache Commons CLI library, version 1.4. You can download the library from the Apache website: [https://commons.apache.org/proper/commons-cli/index.html](https://commons.apache.org/proper/commons-cli/index.html)

I have only tested this on Linux (specifically Ubuntu). It should work fine on other systems but I have not tried it myself.

# Instructions for building and running on Linux
1. Create a build directory inside this directory: ```mkdir build```
2. ```cd build```
3. Download `commons-cli-1.4-bin.tar.gz` from the Apache website: [https://commons.apache.org/proper/commons-cli/download_cli.cgi](https://commons.apache.org/proper/commons-cli/download_cli.cgi)
4. Compare the signatures of the downloaded file against the signatures on the Apache website. This step is optional but highly recommended.
5. Extract the files: ```tar -xzvf commons-cli-1.4-bin.tar.gz```
6. Compile with javac: ```javac -classpath commons-cli-1.4/commons-cli-1.4.jar -sourcepath .. -d . -Xlint ../Escape.java```
7. Run using the java command: ```java -ea -classpath .:commons-cli-1.4/commons-cli-1.4.jar Escape [args]...```

## Testing with integration_tests.py
I have included the script `run.sh` specifically to allow me to run the integration tests with the Java implementation.
1. Build the program as specified above.
2. Make sure the script is executable. If needed, do ```chmod u+x ../run.sh```
3. ```python3 ../../test/integration_tests.py ../run.sh```
