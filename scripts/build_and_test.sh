#!/bin/bash
set -e

mkdir -p build
cd build

cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=OFF
make -j$(nproc)

ctest --verbose