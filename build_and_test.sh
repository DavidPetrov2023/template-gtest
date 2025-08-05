#!/bin/bash
set -e

echo "🔧 Building project and running tests..."

mkdir -p build
cd build
cmake ..
cmake --build .
ctest --output-on-failure
