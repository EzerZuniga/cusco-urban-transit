#!/usr/bin/env bash

# Build script for Urban Transport System
# Usage: ./scripts/build.sh [debug|release]

set -euo pipefail

BUILD_TYPE="${1:-release}"
BUILD_DIR="build"

BUILD_TYPE_LOWER=$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')
if [ "$BUILD_TYPE_LOWER" = "debug" ]; then
    CMAKE_CONFIG=Debug
else
    CMAKE_CONFIG=Release
fi

echo "Building Urban Transport System ($CMAKE_CONFIG)"

if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
    echo "Created build directory: $BUILD_DIR"
fi

cd "$BUILD_DIR"

# Configure CMake
cmake -DCMAKE_BUILD_TYPE=$CMAKE_CONFIG ..

# Determine parallel jobs (portable)
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

echo "Building project (jobs=$JOBS)..."
cmake --build . --config $CMAKE_CONFIG -- -j$JOBS

echo "Build completed ($CMAKE_CONFIG)"
echo "Executable: $BUILD_DIR/urban-transport-system"