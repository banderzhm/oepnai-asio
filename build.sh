#!/bin/bash
# Build script for Linux/macOS

set -e

echo "==================================="
echo "OpenAI Asio - Build Script"
echo "==================================="

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found. Please install CMake 3.28 or later."
    exit 1
fi

# Detect compiler
if command -v clang++-18 &> /dev/null; then
    export CXX=clang++-18
    echo "Using Clang 18"
elif command -v clang++-19 &> /dev/null; then
    export CXX=clang++-19
    echo "Using Clang 19"
elif command -v clang++-20 &> /dev/null; then
    export CXX=clang++-20
    echo "Using Clang 20"
else
    echo "Warning: Clang 18+ not found. Using default compiler."
fi

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "Building..."
cmake --build . -j$(nproc)

echo ""
echo "==================================="
echo "Build completed successfully!"
echo "==================================="
echo ""
echo "To run the examples:"
echo "  export OPENAI_API_KEY='your-api-key'"
echo "  ./openai_asio                # Sync example"
echo "  ./openai_asio --async        # Async example"
echo "  ./openai_asio --http-test    # HTTP client test"
echo ""

