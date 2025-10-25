#!/bin/bash -e

# Script to build and run tests

echo "Building tests..."

# Clean previous build
if [ -d "build_test" ]; then
  rm -rf "build_test"
fi

mkdir "build_test"
cd build_test || exit

# Configure with CMake (tests only, skip main project dependencies)
cmake -DBUILD_TESTS_ONLY=ON ..

# Build
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Running tests..."
echo "================================"

# Run tests
cd tests
ctest --output-on-failure

echo ""
echo "================================"
echo "Tests completed!"
