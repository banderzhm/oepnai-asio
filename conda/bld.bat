@echo off
setlocal enabledelayedexpansion

:: Detect MSVC module directory
set "VCTOOLS_DIR=%VCToolsInstallDir%"
if "%VCTOOLS_DIR%"=="" (
    echo Error: VCToolsInstallDir not set
    exit /b 1
)

set "LIBCXX_MODULE_DIRS=%VCTOOLS_DIR%modules"
echo Using MSVC modules at: %LIBCXX_MODULE_DIRS%

:: Configure with CMake
cmake -B build ^
    -G "Visual Studio 17 2022" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX="%LIBRARY_PREFIX%" ^
    -DCMAKE_PREFIX_PATH="%LIBRARY_PREFIX%" ^
    -DOPENSSL_ROOT_DIR="%LIBRARY_PREFIX%" ^
    -DLIBCXX_MODULE_DIRS="%LIBCXX_MODULE_DIRS%" ^
    -DCMAKE_CXX_STANDARD=23 ^
    -DCMAKE_CXX_SCAN_FOR_MODULES=ON ^
    -S .

if errorlevel 1 exit /b 1

:: Build
cmake --build build --config Release --parallel %CPU_COUNT%
if errorlevel 1 exit /b 1

:: Install
cmake --install build --config Release
if errorlevel 1 exit /b 1

endlocal

