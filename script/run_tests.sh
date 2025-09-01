#!/bin/bash

# Test runner script for Survive game
# Usage: ./run_tests.sh [filter]

cd "$(git rev-parse --show-toplevel)"

echo "=== Survive Game Test Suite ==="
echo "Building project..."

# Build the project
if ! cmake --build build; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ Build successful!"
echo ""

# Run tests
echo "Running tests..."
if [ $# -eq 0 ]; then
    # Run all tests
    cd build && ./SurviveTests
else
    # Run with filter
    echo "Filter: $1"
    cd build && ./SurviveTests "$1"
fi

test_result=$?

if [ $test_result -eq 0 ]; then
    echo ""
    echo "✅ All tests passed!"
else
    echo ""
    echo "❌ Some tests failed!"
fi

exit $test_result
