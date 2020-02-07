#!/bin/bash

# https://sipb.mit.edu/doc/safe-shell/
set -euo pipefail

mkdir -p build && cd build
cmake -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..
# $(nproc) prints number of processing units available
cmake --build . --config Debug -- -j $(nproc)

# run tests
ctest -j $(nproc) --output-on-failure

