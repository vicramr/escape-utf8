# This script is intended to be used by AppVeyor.
# Note: to exit upon an error, we need to actually check the exit code.
# https://stackoverflow.com/a/9949909

echo "GHC version:"
ghc --version

cd $env:APPVEYOR_BUILD_FOLDER\haskell

echo "Now running ghc"
ghc -Wall escape.hs
if ($LastExitCode -ne 0) {
    echo "Error: GHC failed"
    exit 1
}

echo "Now running integration tests"
C:\Python35-x64\python.exe ..\test\integration_tests.py escape.exe
exit $LastExitCode