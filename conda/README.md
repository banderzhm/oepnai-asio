# Conda Package for OpenAI-Asio

This directory contains the conda recipe for building and distributing the openai-asio package.

## Prerequisites

- Anaconda or Miniconda installed
- conda-build package: `conda install conda-build`
- C++23 compatible compiler:
  - Linux: Clang 19+ with libc++
  - macOS: Xcode 15+ (Clang 15+)
  - Windows: Visual Studio 2022 (MSVC 19.40+)

## Building the Conda Package Locally

### 1. Install conda-build

```bash
conda install -n base conda-build
```

### 2. Build the package

From the repository root directory:

```bash
# Linux/macOS
conda build conda --no-test

# Windows
conda build conda --no-test
```

The `--no-test` flag skips tests during the build. Remove it if you want to run tests.

### 3. Install locally

After building, install the package from your local channel:

```bash
# Find the built package location
conda build conda --output

# Install from local build
conda install --use-local openai-asio
```

## Using the Package

### Install from conda-forge (when available)

```bash
conda install -c conda-forge openai-asio
```

### Use in your project

Once installed, you can use the library in your CMake project:

```cmake
find_package(openai-asio REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE openai-asio::openai_asio_core)
```

## Publishing to Conda-Forge

To make this package available on conda-forge:

1. **Fork conda-forge/staged-recipes**: 
   - Go to https://github.com/conda-forge/staged-recipes
   - Fork the repository

2. **Create a new recipe**:
   - Copy `conda/meta.yaml` to `staged-recipes/recipes/openai-asio/meta.yaml`
   - Copy `conda/build.sh` to `staged-recipes/recipes/openai-asio/build.sh`
   - Copy `conda/bld.bat` to `staged-recipes/recipes/openai-asio/bld.bat`

3. **Create Pull Request**:
   - Commit your changes
   - Push to your fork
   - Create a PR to conda-forge/staged-recipes
   - Wait for CI checks to pass
   - Address reviewer feedback

4. **Maintenance**:
   - After acceptance, a new repository will be created: `conda-forge/openai-asio-feedstock`
   - You'll be added as a maintainer
   - Future updates go through that feedstock repository

## Testing the Recipe

Before submitting to conda-forge, test the recipe locally:

```bash
# Build and test
conda build conda

# Test in a clean environment
conda create -n test-env openai-asio --use-local
conda activate test-env

# Try using the library
cd /path/to/your/test/project
cmake -B build
cmake --build build
```

## Supported Platforms

- ✅ Linux (x86_64) with Clang 19+
- ✅ macOS (x86_64, arm64) with Xcode 15+
- ✅ Windows (x86_64) with Visual Studio 2022

## Dependencies

The conda package automatically handles these dependencies:

- OpenSSL ≥ 3.0
- fmt ≥ 10.0
- nlohmann_json ≥ 3.11
- CMake ≥ 4.0 (build-time)
- C++23 compiler (build-time)

## Troubleshooting

### Build fails with "C++23 not supported"

Make sure you have a recent enough compiler:
- Clang 19+ on Linux
- Xcode 15+ on macOS
- Visual Studio 2022 on Windows

### Module errors on Linux

Ensure you have the correct libc++ modules:

```bash
# Ubuntu/Debian
sudo apt-get install libc++-21-dev libc++abi-21-dev

# Check module availability
ls /usr/lib/llvm-21/share/libc++/v1/
```

### OpenSSL not found

Make sure OpenSSL is installed in your conda environment:

```bash
conda install openssl
```

## Contact

For issues related to the conda package, please open an issue at:
https://github.com/banderzhm/oepnai-asio/issues

