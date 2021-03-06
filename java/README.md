This directory contains a Java implementation of the escape program. This code is written for Java 8 and I cannot guarantee that it will work for any other version of Java. This implementation has the exact same command-line interface as the C++ implementation, and the exit codes are also mostly identical. A few error messages are slightly different, but other than that they should be functionally equivalent.

This project uses the Apache Commons CLI library, version 1.4. You can download the library from the Apache website: [https://commons.apache.org/proper/commons-cli/index.html](https://commons.apache.org/proper/commons-cli/index.html)

I have tested this code on Linux, macOS, and Windows.

The files in the ci directory are only intended for use with Travis CI/Appveyor.

# Instructions for building and running
1. Create a build directory inside this directory: ```mkdir build```
2. ```cd build```
3. Download `commons-cli-1.4-bin.tar.gz` or `commons-cli-1.4-bin.zip` from the Apache website: [https://commons.apache.org/proper/commons-cli/download_cli.cgi](https://commons.apache.org/proper/commons-cli/download_cli.cgi)
4. Compare the signatures of the downloaded file against the signatures on the Apache website. This step is optional but highly recommended.
5. Extract the files. Note: the directory `commons-cli-1.4` should be directly inside the `build` directory, so you may have to move it there from `commons-cli-1.4-bin`.
6. Compile with javac: ```javac -classpath commons-cli-1.4/commons-cli-1.4.jar -sourcepath .. -d . -Xlint ../Escape.java```
7. Run using the java command. On Unix-like systems the command is: ```java -classpath .:commons-cli-1.4/commons-cli-1.4.jar Escape [args]...``` and on Windows the command is: ```java -classpath ".;commons-cli-1.4\commons-cli-1.4.jar" Escape [args]...```

## Testing with integration_tests.py
I have included the script `run.sh` specifically to allow me to run the integration tests with the Java implementation.
1. Build the program as specified above, and make sure you are in the `build` directory.
2. Make sure the script is executable. If needed, do ```chmod u+x ../run.sh```
3. ```python3 ../../test/integration_tests.py ../run.sh```

On Windows, you would instead compile `winrun.c` and use the resulting executable in place of `run.sh`. See `ci/win_ciscript.ps1` for an example of how to do this.
