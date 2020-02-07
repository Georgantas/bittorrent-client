#!/bin/bash

# https://sipb.mit.edu/doc/safe-shell/
set -euo pipefail

# INSTALL GTEST FOR TRAVIS
tree
cd libraries/googletest
cmake .
cmake --build .
cmake --install .
cd -
##########################

mkdir -p build && cd build
cmake -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..
# $(nproc) prints number of processing units available
cmake --build . --config Debug -- -j $(nproc)

# run tests
ctest -j $(nproc) --output-on-failure

# run unit tests
./unittests
