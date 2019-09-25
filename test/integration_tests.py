"""
This file contains integration tests. To test the program end-to-end, we use subprocess.Popen
to run the compiled program with various file/text inputs and check that every byte of the output
is exactly as expected (both std/file output and stderr).
"""

import sys
if (sys.version_info[0] < 3) and (sys.version_info[1] < 5):
    sys.exit("This script requires Python 3.5 or above.")

import os
from subprocess import Popen, PIPE

if __name__ == "__main__":
    if len(sys.argv <= 1):
        sys.exit("You must give the path to the compiled 'escape' executable file as a command-line argument.")
    executable = sys.argv[1]

    # To construct the path to the test cases, we can use sys.argv[0]. This code is repurposed from
    # another project of mine:
    # https://github.com/vicramr/fashion-mnist/blob/de31b45a341e1890e96f250106f0679c342b4925/drivers/initialize.py#L35
    arg0 = sys.argv[0]
    if not os.path.isfile(arg0):
        sys.exit("sys.argv[0] is not a path to a file: \"" + str(arg0) + "\". Exiting now.")
    absolute_path_to_file = os.path.realpath(arg0)
    absolute_path_to_test = os.path.dirname(absolute_path_to_file)
    absolute_path_to_vcs_testcases = os.path.join(absolute_path_to_test, "vcs_testcases")

    # simple1
    simple1 = os.path.join(absolute_path_to_vcs_testcases, "simple1")
    with Popen([executable, simple1], stdout=PIPE, stderr=PIPE, universal_newlines=False) as proc:
        (stdout_data, stderr_data) = proc.communicate()
        assert stdout_data == b"lorem ipsum"
        assert stderr_data == b""
