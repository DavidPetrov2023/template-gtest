#!/bin/bash

echo "🔍 Checking test environment for GoogleTest project"
echo "---------------------------------------------------"

check_command() {
    if command -v "$1" &> /dev/null; then
        version=$($1 --version | head -n 1)
        echo "✅ $1 found: $version"
    else
        echo "❌ $1 not found!"
        MISSING=1
    fi
}

MISSING=0

check_command g++
check_command cmake
check_command git

[[ -f "CMakeLists.txt" ]] && echo "✅ CMakeLists.txt found" || { echo "❌ CMakeLists.txt not found"; MISSING=1; }
[[ -d "src" ]] && echo "✅ src/ directory found" || { echo "❌ src/ directory missing"; MISSING=1; }
[[ -d "tests" ]] && echo "✅ tests/ directory found" || { echo "❌ tests/ directory missing"; MISSING=1; }

[[ -d "build" ]] && echo "ℹ️  build/ directory exists" || echo "ℹ️  build/ directory not found (will be created)"

echo "---------------------------------------------------"

if [[ "$MISSING" -eq 1 ]]; then
    echo "❌ Environment check failed. Please fix above issues before proceeding."
    exit 1
else
    echo "✅ Environment check passed. You can proceed with building and testing."
    exit 0
fi
