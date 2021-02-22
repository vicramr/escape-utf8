[![Travis Status](https://travis-ci.com/vicramr/escape-utf8.svg?branch=master)](https://travis-ci.com/vicramr/escape-utf8)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/ejl1jphdn45exq94/branch/master?svg=true)](https://ci.appveyor.com/project/vicramr/escape-utf8/branch/master)

# escape-utf8
Transform UTF-8 text to a representation in ASCII.

This project is a command-line application which takes as input a piece of UTF-8 text and returns the same text, except with non-ASCII characters (and most non-printable ASCII characters) replaced with a human-readable ASCII representation.

There is a C++ implementation (in the `src` directory) and a Java implementation (in the `java` directory). The C++ implementation is the primary implementation. This README focuses on the C++ implementation; see `java/README.md` for details on the Java version. Both implementations are functionally equivalent, except for some minor differences in exit codes, error messages, etc.

## Building and running
This code has been tested on Linux, macOS, and Windows. See `.travis.yml` and `appveyor.yml` for details on the testing setups. This project should work correctly on any Unix-like system and on any relatively modern Windows system.

### Building
The C++ implementation uses CMake as its build system. You must also have a C++ compiler which supports the C++14 standard. There are no other dependencies.

As with any other CMake project, it is recommended that you do an out-of-source build. Instructions:
1. Create a new directory within the root directory of this repository and change your working directory to it.
2. Run the command ```cmake ..```
3. Run the command ```cmake --build . --target escape```

Note that if you are using a multi-configuration build system, the command in step 3 will build the project in debug mode. To build in release mode you should instead do ```cmake --build . --config Release --target escape```. This applies for Visual Studio on Windows, which is multi-configuration. This usually does not apply for Linux, macOS, or other Unix-like systems.

### Running
The build commands will generate an executable called `escape` (or `escape.exe` on Windows). The usage syntax is:

```
escape [INPUTFILE] [-o OUTPUTFILE]
escape -h | --help
escape -v | --version
```

`INPUTFILE` and `OUTPUTFILE` are the input/output filenames and they are both optional. If either is not given, the program will read from stdin/stdout, respectively.

## Details
### Escape format
This project follows the guidelines given in [RFC 5137](https://tools.ietf.org/html/rfc5137) regarding the escape format. Specifically, this project uses the format specified in section 5.1 of RFC 5137 ("Backslash-U with Delimiters").

Here are some examples of the escape format. The Unicode character U+00F1 (lowercase n with a tilde above it) would
be expanded to the 8-character string `\u'00F1'`. U+1F602 (the laughing crying emoji, officially known as "Face with
Tears of Joy") would be expanded to the 9-character string `\u'1F602'`. No extra whitespace is added before or after the escape string.

### Escaped characters
All US-ASCII characters normally considered printable are preserved without any escaping. This is the set of characters with decimal values in the range [32, 126], inclusive. In addition, all tab (\t), line feed (\n), carriage return (\r), and space characters are preserved. All other US-ASCII characters are escaped, as are all Unicode characters outside this range. The vertical tab (\v) and form feed (\f) characters are escaped, even though these characters are in the US-ASCII range and are considered whitespace characters by Unicode.

The byte-order mark (U+FEFF) is escaped just as any other character outside the US-ASCII range would be, even if it is at the beginning of the file.

## Running tests
This project has two dependencies for testing:
* [Catch2](https://github.com/catchorg/Catch2), which is a submodule of this project, is used for the unit tests.
* Python (version 3.5 or above) is used to run the integration tests.

### Unit tests
To build and run the unit tests:
1. Make sure that you have checked out the Catch2 submodule by running ```git submodule update --init --recursive```
2. Follow the instructions above for building the project, but use `--target runtest` instead of `--target escape`.
3. Run the `runtest` executable.

### Integration tests
The Python script `test/integration_tests.py` runs the integration tests. This script takes the built `escape` executable as input and gives it various test cases. To run the integration tests:
1. Build the project using the instructions above.
2. Run ```python3 path/to/test/integration_test.py path/to/escape```

The integration tests will run properly no matter what your current working directory is. However, the integration tests will create several files in your current working directory, **potentially overwriting existing files**. To be safe, you should run the integration tests in a directory without any important files.

## License information
This project is distributed under the terms of the MIT license. See the LICENSE file for details.

This project also uses [Catch2](https://github.com/catchorg/Catch2) as a Git submodule. Catch2 is distributed under the Boost Software License version 1.0.
