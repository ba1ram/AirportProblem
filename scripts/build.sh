#!/bin/bash
set -e

mkdir -p build
cd build

cmake .. -DBUILD_TESTS=OFF -DENABLE_COVERAGE=OFF
make -j$(nproc)