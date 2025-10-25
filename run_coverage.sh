#!/bin/bash -e

# Script to run tests with code coverage and generate reports for SonarQube

echo "Building tests with coverage instrumentation..."

# Clean previous build
if [ -d "build_coverage" ]; then
  rm -rf "build_coverage"
fi

mkdir "build_coverage"
cd build_coverage || exit

# Configure with coverage flags (tests only, skip main project dependencies)
cmake -DCMAKE_BUILD_TYPE=Coverage \
      -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
      -DCMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
      -DBUILD_TESTS_ONLY=ON \
      ..

# Build
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Running tests..."
echo "================================"

# Run tests
cd tests
ctest --output-on-failure

echo ""
echo "Generating coverage report..."
echo "================================"

# Check if lcov is installed
if ! command -v lcov &> /dev/null; then
    echo "WARNING: lcov not found. Installing lcov is recommended for coverage reports."
    echo "On macOS: brew install lcov"
    echo "On Ubuntu: sudo apt-get install lcov"
    echo ""
    echo "Generating basic gcov reports instead..."

    # Generate basic gcov reports
    find . -name "*.gcda" -exec gcov {} \;

    echo ""
    echo "Coverage data files (.gcov) generated in build_coverage/tests/"
    echo "You can analyze these manually or install lcov for better reports."
else
    # Go back to build_coverage root
    cd ..

    # Capture coverage data with error suppression for compatibility
    lcov --capture \
         --directory . \
         --output-file coverage.info \
         --ignore-errors inconsistent,negative,mismatch,format \
         --rc derive_function_end_line=0 \
         --rc geninfo_auto_base=1

    # Remove system, test, and external library files from coverage
    lcov --remove coverage.info \
         '/usr/*' \
         '/Applications/*' \
         '*/googletest/*' \
         '*/gtest/*' \
         '*/gmock/*' \
         '*/_deps/*' \
         '*/tests/*' \
         '*/build_coverage/*' \
         '*/c++/*' \
         --output-file coverage_filtered.info \
         --ignore-errors unused

    # Generate HTML report
    genhtml coverage_filtered.info \
            --output-directory coverage_report \
            --ignore-errors source

    echo ""
    echo "================================"
    echo "Coverage report generated!"
    echo "HTML report: build_coverage/coverage_report/index.html"
    echo "LCOV report (for SonarQube): build_coverage/coverage_filtered.info"
    echo ""
    echo "To view the HTML report:"
    echo "  open build_coverage/coverage_report/index.html"
    echo ""
    echo "For SonarQube, use the coverage_filtered.info file"
fi
