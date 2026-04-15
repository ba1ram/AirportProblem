#!/bin/bash
set -e

mkdir -p build
cd build

cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON
make -j$(nproc)

ctest --verbose

lcov --capture --directory . --output-file coverage.info

lcov --remove coverage.info \
    '/usr/*' \
    '*/tests/*' \
    '*/_deps/*' \
    '*/gtest/*' \
    --output-file coverage_clean.info

genhtml coverage_clean.info --output-directory coverage_report

echo "Open: build/coverage_report/index.html"