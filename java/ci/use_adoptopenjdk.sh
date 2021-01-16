# This script is intended to be sourced by Travis CI when using AdoptOpenJDK.
# It is based on the instructions here: https://github.com/AdoptOpenJDK/homebrew-openjdk#switch-between-different-jdk-versions
JAVA_HOME=$(/usr/libexec/java_home -v1.8)
export JAVA_HOME
