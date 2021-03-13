# This script is intended to be used by AppVeyor.
# The working directory is assumed to be the repo's root directory.
# Note: to exit upon an error, we need to actually check the exit code.
# https://stackoverflow.com/a/9949909

echo "GHC version:"
ghc --version

echo "Now running ghc"
ghc -Wall escape.hs
if ($LastExitCode -ne 0) {
    echo "Error: GHC failed"
    exit $LastExitCode
}

echo "Now running integration tests"
C:\Python35-x64\python.exe ..\test\integration_tests.py escape.exe
exit $LastExitCode
