"""
NOTE: this program will create new files in the current directory, potentially overwriting
existing files. Be careful!

This file contains integration tests. To test the program end-to-end, we use subprocess.Popen
to run the compiled program with various file/text inputs and check that every byte of the output
is exactly as expected (both std/file output and stderr).

Unfortunately, because the cin/cout streams are opened in text mode by default, it is hard
to write byte-for-byte tests that use stdin or stdout. That's because line endings will be
altered by cin/cout on Windows but not on Unix-like systems. The workaround is to specify
both an input and output file for any test cases that involve line endings.
"""

import sys
if (sys.version_info[0] < 3) or (sys.version_info[0] == 3 and sys.version_info[1] < 5):
    sys.exit("This script requires Python 3.5 or above.")

import os
from subprocess import Popen, PIPE
from codecs import encode

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        sys.exit("You must give the path to the compiled 'escape' executable file as a command-line argument.")
    executable = sys.argv[1]
    absolute_path_to_executable = os.path.realpath(executable)

    # To construct the path to the test cases, we can use sys.argv[0]. This code is repurposed from
    # another project of mine:
    # https://github.com/vicramr/fashion-mnist/blob/de31b45a341e1890e96f250106f0679c342b4925/drivers/initialize.py#L35
    arg0 = sys.argv[0]
    if not os.path.isfile(arg0):
        sys.exit("sys.argv[0] is not a path to a file: \"" + str(arg0) + "\". Exiting now.")
    absolute_path_to_file = os.path.realpath(arg0)
    absolute_path_to_test = os.path.dirname(absolute_path_to_file)
    absolute_path_to_vcs_testcases = os.path.join(absolute_path_to_test, "vcs_testcases")
    absolute_path_to_gen = os.path.join(absolute_path_to_vcs_testcases, "gen")

    # simple1
    simple1 = os.path.join(absolute_path_to_vcs_testcases, "simple1")
    with Popen([absolute_path_to_executable, simple1], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode == 0
        assert stdout_data == b"lorem ipsum"
        assert stderr_data == b""

    # joy
    joy = os.path.join(absolute_path_to_vcs_testcases, "joy")
    with Popen([absolute_path_to_executable, joy], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode == 0
        assert stdout_data == br"\u'1F602'\u'1F602'"
        assert stderr_data == b""

    # Test ASCII control characters
    with Popen([absolute_path_to_executable], stdin=PIPE, stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate(b"\x00")
        assert proc.returncode == 0
        assert stdout_data == br"\u'0000'"
        assert stderr_data == b""

    control = os.path.join(absolute_path_to_gen, "control")
    out = b"foo \\u'0001'bar\r\n\\u'0008'\t\n\\u'000B'\\u'000C'\r\n\\u'001F' ~\\u'007F'"
    with Popen([absolute_path_to_executable, "--output", "control1", control], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode == 0
        assert stdout_data == b""
        assert stderr_data == b""
        with open("control1", mode="rb") as f:
            control1_data = f.read()
            assert control1_data == out


    # crlf
    crlf = os.path.join(absolute_path_to_vcs_testcases, "crlf")
    out = b"This is a plain old ASCII file with CRLF\r\nline endings. Lorem\r\nipsum\r\n"
    with Popen([absolute_path_to_executable, "-ocrlf1", crlf], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode == 0
        assert stdout_data == b""
        assert stderr_data == b""
        with open("crlf1", mode="rb") as f:
            crlf1_data = f.read()
            assert crlf1_data == out

    # hearteyes
    hearteyes = os.path.join(absolute_path_to_vcs_testcases, "hearteyes")
    out = b"\\u'1F60D'\\u'1F60D' \\u'1F60D'  \\u'1F60D'\n"
    with Popen([absolute_path_to_executable, "--output=hearteyes1", hearteyes], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode == 0
        assert stdout_data == b""
        assert stderr_data == b""
        with open("hearteyes1", mode="rb") as f:
            hearteyes1_data = f.read()
            assert hearteyes1_data == out

    # hearteyes, with slightly different command line
    with Popen([absolute_path_to_executable, hearteyes, "--output", "hearteyes2"], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode == 0
        assert stdout_data == b""
        assert stderr_data == b""
        with open("hearteyes2", mode="rb") as f:
            hearteyes2_data = f.read()
            assert hearteyes2_data == out

    # holamundo
    holamundo = os.path.join(absolute_path_to_gen, "holamundo")
    out = b"\\u'00A1'Hola mundo!\n"
    with Popen([absolute_path_to_executable, holamundo, "-o", "holamundo1"], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode == 0
        assert stdout_data == b""
        assert stderr_data == b""
        with open("holamundo1", mode="rb") as f:
            holamundo1_data = f.read()
            assert holamundo1_data == out

    # Latin-1: various characters from the Latin-1 block.
    with Popen([absolute_path_to_executable], stdin=PIPE, stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        input_bytes = encode("\t\u0080 \u00B1Hi World\u00A2  \u00F7\u00FF\t ", encoding="utf8")
        (stdout_data, stderr_data) = proc.communicate(input_bytes)
        assert proc.returncode == 0
        assert stdout_data == b"\t\\u'0080' \\u'00B1'Hi World\\u'00A2'  \\u'00F7'\\u'00FF'\t "
        assert stderr_data == b""

    # shortmix
    shortmix = os.path.join(absolute_path_to_gen, "shortmix")
    out = b"\\u'2020' \\u'0007'\r\n\\u'10904'\\u'FE18'\\u'042F'\r\n\r\n"
    with Popen([absolute_path_to_executable, "-o", "shortmix1", shortmix], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode == 0
        assert stdout_data == b""
        assert stderr_data == b""
        with open("shortmix1", mode="rb") as f:
            shortmix1_data = f.read()
            assert shortmix1_data == out
        print('passed shortmix')

    # Nonexistent input file (note, we're opening the streams in text mode here!)
    with Popen([absolute_path_to_executable, "NonExistentFileName"], stdout=PIPE, stderr=PIPE, universal_newlines=True) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert proc.returncode != 0
        assert stdout_data == ""
        assert stderr_data == 'Failed to open input file "NonExistentFileName". Exiting now.\n'


    print("All integration tests passed!")
