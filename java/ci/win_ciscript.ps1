# This Powershell script is the equivalent of ciscript.sh, for Windows.
# Like ciscript.sh, this assumes you are in the root dir of the repo.
# This script is only intended for use on the Visual Studio 2019 image
# in AppVeyor. As such, it has a hard-coded path to the Python executable.

echo "Running Windows integration tests for Java"
echo "java version:"
java -version 2>&1 | %{ "$_" }
echo "javac version:"
javac -version 2>&1 | %{ "$_" }
# Note: the -version commands print to stderr, which Powershell treats as an error.
# The workaround is to convert the error objects to strings. See here:
# https://stackoverflow.com/a/20950421

cd java
md build
cd build
md commons-cli-1.4
cd commons-cli-1.4

wget https://repo1.maven.org/maven2/commons-cli/commons-cli/1.4/commons-cli-1.4.jar -OutFile commons-cli-1.4.jar

# Code to check hash is modified from here: https://stackoverflow.com/a/63396621
if ((Get-FileHash -Algorithm SHA256 -Path .\commons-cli-1.4.jar).Hash -eq "FD3C7C9545A9CDB2051D1F9155C4F76B1E4AC5A57304404A6EEDB578FFBA7328") {
	cd ..
	echo "Now running javac"
	javac -classpath commons-cli-1.4\commons-cli-1.4.jar -sourcepath .. -d . -Xlint ..\Escape.java
	echo "Now compiling winrun"
	cmake ..\ci\ -DCMAKE_CONFIGURATION_TYPES="Release"
	cmake --build . --config Release
	echo "Now running tests"
	C:\Python35-x64\python.exe ..\..\test\integration_tests.py .\Release\winrun.exe
	exit $LastExitCode
} else {
	echo "Checksum did not match. Exiting now."
	exit 1
}

# See this in-depth answer about returning exit codes:
# https://stackoverflow.com/a/57468523
