cmake_minimum_required(VERSION 3.28)

project(openai-asio 
    VERSION 1.0.0
    DESCRIPTION "OpenAI API Client with Asio - Async C++ Library"
    LANGUAGES CXX
)

# ==============================================================================
# C++ Standard Configuration
# ==============================================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================================================================
# Auto-Detect Platform-Specific Standard Library Module Paths
# ==============================================================================

if(UNIX)
    # Linux/Unix: Auto-detect libc++ standard library module paths
    
    # Try to find libc++ module directory
    find_path(LIBCXX_MODULE_DIRS
        NAMES std.cppm
        PATHS
            /usr/lib/llvm-21/share/libc++/v1
            /usr/lib/llvm-20/share/libc++/v1
            /usr/lib/llvm-19/share/libc++/v1
            /usr/lib/llvm-18/share/libc++/v1
            /usr/local/lib/llvm-21/share/libc++/v1
            /usr/local/lib/llvm-20/share/libc++/v1
            /opt/llvm/share/libc++/v1
        DOC "Path to libc++ standard library modules"
    )
    
    # Try to find libc++ include directory
    find_path(LIBCXX_INCLUDE_DIRS
        NAMES __config
        PATHS
            /usr/lib/llvm-21/include/c++/v1
            /usr/lib/llvm-20/include/c++/v1
            /usr/lib/llvm-19/include/c++/v1
            /usr/lib/llvm-18/include/c++/v1
            /usr/local/lib/llvm-21/include/c++/v1
            /usr/local/lib/llvm-20/include/c++/v1
            /opt/llvm/include/c++/v1
            /usr/include/c++/v1
        DOC "Path to libc++ headers"
    )
    
    if(LIBCXX_MODULE_DIRS AND LIBCXX_INCLUDE_DIRS)
        message(STATUS "Auto-detected libc++ paths:")
        message(STATUS "  Module directory: ${LIBCXX_MODULE_DIRS}")
        message(STATUS "  Include directory: ${LIBCXX_INCLUDE_DIRS}")
        
        # Force use of libc++ for all targets (must be set before adding third-party libraries)
        add_compile_options(
            -nostdinc++
            -isystem ${LIBCXX_INCLUDE_DIRS}
        )
        add_link_options(-stdlib=libc++ -lc++ -lc++abi)
    else()
        message(WARNING "Could not auto-detect libc++ paths. Please set manually:")
        message(WARNING "  cmake -DLIBCXX_MODULE_DIRS=<path> -DLIBCXX_INCLUDE_DIRS=<path> ..")
    endif()
    
elseif(WIN32)
    # Windows: Auto-detect MSVC standard library module paths
    
    # Priority 1: Try VCToolsInstallDir environment variable first
    if(DEFINED ENV{VCToolsInstallDir})
        set(LIBCXX_MODULE_DIRS "$ENV{VCToolsInstallDir}/modules" CACHE PATH "Path to MSVC modules")
        if(EXISTS "${LIBCXX_MODULE_DIRS}/std.ixx")
            message(STATUS "Using MSVC module directory from VCToolsInstallDir: ${LIBCXX_MODULE_DIRS}")
        else()
            message(WARNING "VCToolsInstallDir found but std.ixx not found at: ${LIBCXX_MODULE_DIRS}")
            unset(LIBCXX_MODULE_DIRS CACHE)
        endif()
    endif()
    
    # Priority 2: If environment variable not found or invalid, try to detect from compiler path and common locations
    if(NOT LIBCXX_MODULE_DIRS)
        # Extract MSVC version from compiler path
        get_filename_component(COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        get_filename_component(COMPILER_BIN_DIR "${COMPILER_DIR}" DIRECTORY)
        
        find_path(LIBCXX_MODULE_DIRS
            NAMES std.ixx
            PATHS
                "${COMPILER_BIN_DIR}/modules"
                "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/modules"
                "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.44.35207/modules"
                "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.44.35207/modules"
                "F:/program/visul_studio/idea/VC/Tools/MSVC/14.44.35207/modules"
            DOC "Path to MSVC standard library modules"
        )
        
        if(LIBCXX_MODULE_DIRS)
            message(STATUS "Auto-detected MSVC module directory: ${LIBCXX_MODULE_DIRS}")
        else()
            message(WARNING "Could not auto-detect MSVC module path. Please set manually:")
            message(WARNING "  cmake -DLIBCXX_MODULE_DIRS=<path> ..")
        endif()
    endif()
endif()

# ==============================================================================
# Third-Party Library Configuration
# ==============================================================================

# Find OpenSSL BEFORE adding asio (asio needs it to enable SSL support)
find_package(OpenSSL REQUIRED)
message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")

# Temporarily disable module scanning for third-party libraries to avoid
# CMake trying to scan their compiler feature tests as modules
set(CMAKE_CXX_SCAN_FOR_MODULES_BACKUP ${CMAKE_CXX_SCAN_FOR_MODULES})
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Enable C++ module support for third-party libraries
set(FMT_MODULE ON CACHE BOOL "Build fmt as a C++ module" FORCE)
set(NLOHMANN_JSON_BUILD_MODULES ON CACHE BOOL "Build nlohmann_json C++ module" FORCE)
# SSL support for Asio module (default is ON, but explicitly set for clarity)
set(ASIO_ENABLE_SSL ON CACHE BOOL "Enable SSL support for ASIO module" FORCE)

# Add third-party libraries
add_subdirectory(3rdparty/fmt)
add_subdirectory(3rdparty/nlohmann_json)
add_subdirectory(3rdparty/asio)  # This will use OpenSSL and set ASIO_USE_SSL internally

# Re-enable module scanning for our library
set(CMAKE_CXX_SCAN_FOR_MODULES ${CMAKE_CXX_SCAN_FOR_MODULES_BACKUP})

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nlohmann_json/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/asio/asio/include)

# ==============================================================================
# Source Files Collection
# ==============================================================================

# Collect all C++ source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")

# Collect module interface files
file(GLOB_RECURSE MODULE_INTERFACE_FILES 
    "src/*.cppm"    # C++23 module interface files
    "src/*.ixx"     # MSVC module interface files
)

# ==============================================================================
# Library Target
# ==============================================================================

# Create static library containing all modules and implementations
add_library(openai_asio_core STATIC 
    ${CPP_SOURCES} 
    ${MODULE_INTERFACE_FILES}
)

# Platform-specific module configuration
if(UNIX AND LIBCXX_MODULE_DIRS AND LIBCXX_INCLUDE_DIRS)
    # Add libc++ include directory (system-level, won't override other paths)
    target_include_directories(openai_asio_core SYSTEM PUBLIC
        ${LIBCXX_INCLUDE_DIRS}
    )
    
    # Add all modules to FILE_SET (avoid overwriting)
    target_sources(openai_asio_core PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${LIBCXX_MODULE_DIRS}
        FILES 
            ${MODULE_INTERFACE_FILES}
            ${LIBCXX_MODULE_DIRS}/std.cppm 
            ${LIBCXX_MODULE_DIRS}/std.compat.cppm
    )
    
elseif(WIN32 AND LIBCXX_MODULE_DIRS)
    # Add all modules to FILE_SET (avoid overwriting)
    target_sources(openai_asio_core PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${LIBCXX_MODULE_DIRS}
        FILES 
            ${MODULE_INTERFACE_FILES}
            ${LIBCXX_MODULE_DIRS}/std.ixx 
            ${LIBCXX_MODULE_DIRS}/std.compat.ixx
    )
else()
    message(WARNING "Standard library modules not configured. Module support may be limited.")
endif()

# Link third-party libraries to core library
# Add OpenSSL include directories
target_include_directories(openai_asio_core SYSTEM PRIVATE
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(openai_asio_core PUBLIC
    fmt::fmt
    nlohmann_json_modules
    asio::asio
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Windows platform requires network libraries
if(WIN32)
    target_link_libraries(openai_asio_core PUBLIC ws2_32 mswsock)
endif()

# Module generation mode (CMake 4.0+ feature)
# SEPARATE: Module interface units are compiled separately from other translation units
set_property(TARGET openai_asio_core PROPERTY
    CXX_MODULE_GENERATION_MODE "SEPARATE"
)

# ==============================================================================
# Main Library Target (Alias for openai_asio_core)
# ==============================================================================

# Create an alias for easier usage
add_library(openai_asio ALIAS openai_asio_core)

# ==============================================================================
# Examples Configuration
# ==============================================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/example/CMakeLists.txt")
    add_subdirectory(example)
    message(STATUS "Examples enabled in example/ directory")
endif()

# ==============================================================================
# Installation Configuration
# ==============================================================================

# Install library
install(TARGETS openai_asio_core
    EXPORT openai-asio-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET cxx_modules DESTINATION lib/cxx/miu
)

# Install third-party libraries
install(TARGETS fmt nlohmann_json_modules asio
    EXPORT openai-asio-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install module files
install(DIRECTORY src/
    DESTINATION include/openai-asio/modules
    FILES_MATCHING 
    PATTERN "*.cppm"
    PATTERN "*.ixx"
)

# Create and install package configuration files
include(CMakePackageConfigHelpers)

# Generate package config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/openai-asio-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/openai-asio-config.cmake"
    INSTALL_DESTINATION lib/cmake/openai-asio
)

# Generate package version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/openai-asio-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install CMake config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/openai-asio-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/openai-asio-config-version.cmake"
    DESTINATION lib/cmake/openai-asio
)

# Install targets file
install(EXPORT openai-asio-targets
    FILE openai-asio-targets.cmake
    NAMESPACE openai-asio::
    DESTINATION lib/cmake/openai-asio
)

# Install license and readme
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
    DESTINATION share/openai-asio
)

# ==============================================================================
# Build Summary
# ==============================================================================

message(STATUS "==========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(UNIX)
    if(LIBCXX_MODULE_DIRS)
        message(STATUS "libc++ Modules: ${LIBCXX_MODULE_DIRS}")
    endif()
elseif(WIN32)
    if(LIBCXX_MODULE_DIRS)
        message(STATUS "MSVC Modules: ${LIBCXX_MODULE_DIRS}")
    endif()
endif()
message(STATUS "==========================================")