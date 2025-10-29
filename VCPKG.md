# vcpkg Support for OpenAI-Asio

This document explains how to use openai-asio with vcpkg package manager.

## Quick Start with vcpkg

### Method 1: Using vcpkg Manifest Mode (Recommended)

1. Create or update your `vcpkg.json` in your project:

```json
{
  "dependencies": [
    {
      "name": "openai-asio",
      "version>=": "1.0.0"
    }
  ]
}
```

2. Configure your CMake project:

```bash
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```

3. Build your project:

```bash
cmake --build build
```

### Method 2: Classic Mode

```bash
# Install the package
vcpkg install openai-asio

# Use in CMake
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```

## Platform-Specific Installation

### Windows (MSVC)

```powershell
# Install for x64-windows
vcpkg install openai-asio:x64-windows

# Or for static CRT
vcpkg install openai-asio:x64-windows-static-md
```

### Linux (Clang with libc++)

```bash
# Install for x64-linux
vcpkg install openai-asio:x64-linux

# Ensure Clang 19+ is installed
sudo apt install clang-19 libc++-19-dev libc++abi-19-dev
```

### macOS

```bash
# Install for x64-osx or arm64-osx
vcpkg install openai-asio:arm64-osx

# Or for x64
vcpkg install openai-asio:x64-osx
```

## Using in Your CMake Project

### Basic Usage

```cmake
cmake_minimum_required(VERSION 4.0)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Find the package
find_package(openai-asio CONFIG REQUIRED)

# Link to your target
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE openai-asio::openai-asio)
```

### With C++ Modules

```cmake
# Enable module scanning
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Your target
add_executable(my_app)
target_sources(my_app PRIVATE
    main.cpp
)

# Link openai-asio (provides modules automatically)
target_link_libraries(my_app PRIVATE openai-asio::openai-asio)
```

## Submitting to vcpkg Official Registry

To submit openai-asio to the official vcpkg registry:

### 1. Create Port Files

Create the following structure:

```
ports/openai-asio/
├── portfile.cmake
├── vcpkg.json
└── usage
```

#### `portfile.cmake`

```cmake
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO banderzhm/oepnai-asio
    REF v${VERSION}
    SHA512 <compute SHA512 of release tarball>
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_SCAN_FOR_MODULES=ON
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/openai-asio)
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(INSTALL "${SOURCE_PATH}/LICENSE" 
     DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" 
     RENAME copyright)
```

#### `vcpkg.json` (for port)

```json
{
  "name": "openai-asio",
  "version": "1.0.0",
  "description": "OpenAI API Client with Asio - Modern C++23 library with coroutines and modules",
  "homepage": "https://github.com/banderzhm/oepnai-asio",
  "license": "MIT",
  "supports": "!(uwp | arm)",
  "dependencies": [
    {
      "name": "vcpkg-cmake",
      "host": true
    },
    {
      "name": "vcpkg-cmake-config",
      "host": true
    },
    "openssl",
    "fmt",
    "nlohmann-json"
  ]
}
```

#### `usage`

```
openai-asio provides CMake targets:

    find_package(openai-asio CONFIG REQUIRED)
    target_link_libraries(main PRIVATE openai-asio::openai-asio)
```

### 2. Submit Pull Request

1. Fork [microsoft/vcpkg](https://github.com/microsoft/vcpkg)
2. Create a new branch: `git checkout -b add-openai-asio`
3. Add your port files to `ports/openai-asio/`
4. Update `versions/baseline.json` and `versions/o-/openai-asio.json`
5. Test the port:
   ```bash
   vcpkg install openai-asio --overlay-ports=./ports/openai-asio
   ```
6. Submit PR to vcpkg repository

### 3. PR Requirements

- [ ] Port builds successfully on all platforms (Windows, Linux, macOS)
- [ ] All tests pass
- [ ] License file is correctly installed
- [ ] CMake config files work properly
- [ ] No warnings during build
- [ ] Follows vcpkg [contributing guidelines](https://github.com/microsoft/vcpkg/blob/master/docs/contributing.md)

## vcpkg.json Features

The project's `vcpkg.json` supports the following features:

### Examples Feature

Build and install example programs:

```bash
vcpkg install openai-asio[examples]
```

## Triplets

Recommended triplets for different platforms:

- **Windows MSVC**: `x64-windows-static-md`
- **Linux Clang**: `x64-linux`
- **macOS**: `arm64-osx` or `x64-osx`

## Compiler Requirements

| Platform | Compiler | Minimum Version |
|----------|----------|----------------|
| Windows  | MSVC     | 19.40 (VS 2022) |
| Linux    | Clang    | 19             |
| macOS    | Clang    | 15             |

## Environment Variables

### Windows

```powershell
$env:VCPKG_ROOT = "C:\vcpkg"
$env:VCToolsInstallDir = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\"
```

### Linux/macOS

```bash
export VCPKG_ROOT=/path/to/vcpkg
export LIBCXX_MODULE_DIRS=/usr/lib/llvm-21/share/libc++/v1
export LIBCXX_INCLUDE_DIRS=/usr/lib/llvm-21/include/c++/v1
```

## Troubleshooting

### CMake Can't Find Package

```bash
# Verify installation
vcpkg list | grep openai-asio

# Check CMake can find it
cmake --find-package \
  -DNAME=openai-asio \
  -DCOMPILER_ID=Clang \
  -DLANGUAGE=CXX \
  -DMODE=EXIST \
  -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
```

### Module Compilation Errors

Ensure you have:
1. CMake 4.0+
2. C++23 compatible compiler
3. Module scanning enabled: `-DCMAKE_CXX_SCAN_FOR_MODULES=ON`

### OpenSSL Not Found

```bash
# Install OpenSSL via vcpkg
vcpkg install openssl

# Or specify OpenSSL path
cmake -B build \
  -DOPENSSL_ROOT_DIR=$VCPKG_ROOT/installed/x64-linux
```

## Resources

- [vcpkg Documentation](https://vcpkg.io/)
- [vcpkg GitHub](https://github.com/microsoft/vcpkg)
- [CMake Modules Documentation](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html)

## License

MIT License - See LICENSE file for details.

