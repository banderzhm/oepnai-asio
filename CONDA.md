# Conda Support for OpenAI-Asio

This document explains how to build and install openai-asio using conda.

## Prerequisites

- Conda or Miniconda installed
- conda-build package
- C++23 compatible compiler

## Building with Conda

### 1. Install conda-build

```bash
conda install conda-build
```

### 2. Build the package

From the repository root:

```bash
conda build conda/
```

### 3. Install the built package

```bash
conda install --use-local openai-asio
```

## For Package Maintainers

### Submitting to conda-forge

To submit this package to conda-forge:

1. Fork the [staged-recipes](https://github.com/conda-forge/staged-recipes) repository

2. Copy the recipe from `conda/` to `staged-recipes/recipes/openai-asio/`

3. Update the recipe if needed:
   - Ensure all dependencies are available in conda-forge
   - Update URLs and checksums
   - Test on all platforms

4. Submit a Pull Request to conda-forge/staged-recipes

5. Address any feedback from reviewers

### Recipe Files

- `meta.yaml`: Package metadata and dependencies
- `build.sh`: Linux/macOS build script
- `bld.bat`: Windows build script
- `conda_build_config.yaml`: Build configuration

## Using in Your Project

Once installed via conda, you can use it in your CMake project:

```cmake
find_package(openai-asio REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE openai-asio::openai-asio)
```

## Platform Support

- **Linux**: Requires Clang 19+ with libc++
- **macOS**: Requires Clang 15+ (from Xcode Command Line Tools)
- **Windows**: Requires Visual Studio 2022 (19.40+)

## Compiler Requirements

This library requires C++23 support:

- **Clang**: 19 or later (recommended: 21)
- **MSVC**: 19.40 or later (Visual Studio 2022)
- **GCC**: 13 or later (experimental module support)

## Dependencies

The conda package automatically installs:

- OpenSSL >= 3.0
- fmt >= 10.0
- nlohmann_json >= 3.11

## Environment Setup

### Linux

```bash
# Create a new environment
conda create -n openai-asio-dev clangxx_linux-64=21 cmake ninja
conda activate openai-asio-dev

# Install the package
conda install --use-local openai-asio
```

### macOS

```bash
# Create a new environment
conda create -n openai-asio-dev clangxx_osx-64 cmake ninja
conda activate openai-asio-dev

# Install the package
conda install --use-local openai-asio
```

### Windows

```bash
# Create a new environment
conda create -n openai-asio-dev vs2022_win-64 cmake
conda activate openai-asio-dev

# Install the package
conda install --use-local openai-asio
```

## Testing the Installation

After installation, verify the package:

```bash
# Check installed files
conda list openai-asio

# Verify CMake can find the package
cmake --find-package -DNAME=openai-asio -DCOMPILER_ID=Clang -DLANGUAGE=CXX -DMODE=EXIST
```

## Troubleshooting

### Module Support Issues

If you encounter module compilation errors:

1. Ensure you're using a compatible compiler version
2. Check that CMake version is 4.0+
3. Verify standard library module paths are correctly detected

### OpenSSL Not Found

```bash
# Ensure OpenSSL is installed
conda install openssl

# Check OpenSSL location
echo $CONDA_PREFIX  # Should point to your conda environment
```

### Compiler Version Issues

```bash
# Check compiler version
clang++ --version  # Should be 19+ on Linux, 15+ on macOS
cl.exe            # Should be MSVC 19.40+ on Windows
```

## Contributing

If you find issues with the conda recipe, please:

1. Open an issue on GitHub
2. Submit a PR with fixes
3. Test on all supported platforms

## License

This conda recipe is distributed under the same MIT license as the main project.

