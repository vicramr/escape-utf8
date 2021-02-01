# This Powershell script is the equivalent of ciscript.sh, for Windows.
# Like ciscript.sh, this assumes you are in the root dir of the repo.

echo "Running Windows integration tests for Java"
echo "java version:"
java -version
echo "javac version:"
javac -version

cd java
md build
cd build
md commons-cli-1.4
cd commons-cli-1.4

wget https://repo1.maven.org/maven2/commons-cli/commons-cli/1.4/commons-cli-1.4.jar -OutFile commons-cli-1.4.jar

# Code to check hash is modified from here: https://stackoverflow.com/a/63396621
if ((Get-FileHash -Algorithm SHA256 -Path .\commons-cli-1.4.jar).Hash -eq "FD3C7C9545A9CDB2051D1F9115C4F76B1E4AC5A57304404A6EEDB578FFBA7328") {
	cd ..
	echo "Now running javac"
	javac -classpath commons-cli-1.4/commons-cli-1.4.jar -sourcepath .. -d . -Xlint ../Escape.java
	echo "Now compiling winrun"
	cl /permissive- /W4 /DNDEBUG ..\winrun.c
	echo "Now running tests"
	C:\Python35-x64\python.exe ..\..\test\integration_tests.py ..\winrun.exe
} else {
	echo "Checksum did not match. Exiting now."
	exit 1
}
