#!/bin/bash
# NOTE: this script assumes that the current working directory is your build directory, e.g. java/build
exec java -ea -classpath .:commons-cli-1.4/commons-cli-1.4.jar Escape "$@"
