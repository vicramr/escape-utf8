#!/bin/bash
# This script is used in .travis.yml to run the integration tests on the Java code.

set -e # This is important to stop the script if the checksum doesn't match.
echo 'Running integration tests for Java'
echo 'java version:'
java -version
echo 'javac version:'
javac -version

cd java
mkdir build
cd build
mkdir commons-cli-1.4 # This is needed because run.sh expects this directory hierarchy.
cd commons-cli-1.4

# We must download the jarfile from Maven Central and manually check the SHA256 hash.
# This is the correct hash, computed on my own machine.
echo 'fd3c7c9545a9cdb2051d1f9155c4f76b1e4ac5a57304404a6eedb578ffba7328  commons-cli-1.4.jar' > checksum
wget https://repo1.maven.org/maven2/commons-cli/commons-cli/1.4/commons-cli-1.4.jar
shasum -a 256 -c checksum

cd ..
echo 'Now running javac'
javac -classpath commons-cli-1.4/commons-cli-1.4.jar -sourcepath .. -d . -Xlint ../Escape.java
echo 'Now running tests'
python3 ../../test/integration_tests.py ../run.sh
