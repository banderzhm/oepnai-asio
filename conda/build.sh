#!/bin/bash

set -ex

# Determine compiler and standard library module paths
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux: Auto-detect libc++ paths
    if command -v clang++-21 &> /dev/null; then
        export CXX=clang++-21
        export CC=clang-21
        LIBCXX_MODULE_DIRS=/usr/lib/llvm-21/share/libc++/v1
        LIBCXX_INCLUDE_DIRS=/usr/lib/llvm-21/include/c++/v1
    elif command -v clang++-20 &> /dev/null; then
        export CXX=clang++-20
        export CC=clang-20
        LIBCXX_MODULE_DIRS=/usr/lib/llvm-20/share/libc++/v1
        LIBCXX_INCLUDE_DIRS=/usr/lib/llvm-20/include/c++/v1
    elif command -v clang++-19 &> /dev/null; then
        export CXX=clang++-19
        export CC=clang-19
        LIBCXX_MODULE_DIRS=/usr/lib/llvm-19/share/libc++/v1
        LIBCXX_INCLUDE_DIRS=/usr/lib/llvm-19/include/c++/v1
    else
        echo "Error: No suitable Clang version found (requires 19+)"
        exit 1
    fi
    
    CMAKE_ARGS=(
        -DLIBCXX_MODULE_DIRS="$LIBCXX_MODULE_DIRS"
        -DLIBCXX_INCLUDE_DIRS="$LIBCXX_INCLUDE_DIRS"
        -DCMAKE_CXX_FLAGS="-stdlib=libc++"
    )
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS: Use system Clang
    export CXX=clang++
    export CC=clang
    
    # macOS module paths
    LIBCXX_MODULE_DIRS=/Library/Developer/CommandLineTools/usr/share/libc++/v1
    LIBCXX_INCLUDE_DIRS=/Library/Developer/CommandLineTools/usr/include/c++/v1
    
    CMAKE_ARGS=(
        -DLIBCXX_MODULE_DIRS="$LIBCXX_MODULE_DIRS"
        -DLIBCXX_INCLUDE_DIRS="$LIBCXX_INCLUDE_DIRS"
    )
fi

# Configure with CMake
cmake -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_PREFIX_PATH="$PREFIX" \
    -DOPENSSL_ROOT_DIR="$PREFIX" \
    -DCMAKE_CXX_STANDARD=23 \
    -DCMAKE_CXX_SCAN_FOR_MODULES=ON \
    "${CMAKE_ARGS[@]}" \
    -S .

# Build
cmake --build build --parallel ${CPU_COUNT}

# Install
cmake --install build

