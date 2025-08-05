#!/bin/bash
set -e

echo "🧹 Cleaning previous build..."
rm -rf build

echo "📁 Creating build directory..."
mkdir build
cd build

echo "⚙️ Configuring project with CMake..."
cmake ..

echo "🔨 Building project..."
cmake --build .

echo "🧪 Running tests..."
ctest --output-on-failure
