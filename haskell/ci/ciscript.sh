#!/bin/bash
# This script is intended to be used by Travis CI.
# The working directory is assumed to be the repo's root directory.
set -e

echo "GHC version:"
ghc --version
cd haskell
echo "Now running ghc"
ghc -Wall escape.hs
echo "Now running integration tests"
python3 ../test/integration_tests.py escape
