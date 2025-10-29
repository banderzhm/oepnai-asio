# Build script for Windows (PowerShell)

$ErrorActionPreference = "Stop"

Write-Host "===================================" -ForegroundColor Cyan
Write-Host "OpenAI Asio - Build Script" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""

# Check for CMake
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "Error: CMake not found. Please install CMake 3.28 or later." -ForegroundColor Red
    exit 1
}

# Check for Visual Studio
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vsWhere) {
    $vsPath = & $vsWhere -latest -property installationPath
    if ($vsPath) {
        Write-Host "Found Visual Studio at: $vsPath" -ForegroundColor Green
    }
} else {
    Write-Host "Warning: Visual Studio not found via vswhere" -ForegroundColor Yellow
}

# Create build directory
$BuildDir = "build"
if (Test-Path $BuildDir) {
    Write-Host "Cleaning existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BuildDir
}

New-Item -ItemType Directory -Path $BuildDir | Out-Null
Set-Location $BuildDir

Write-Host ""
Write-Host "Configuring..." -ForegroundColor Cyan
cmake .. `
    -G "Visual Studio 17 2022" `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Building..." -ForegroundColor Cyan
cmake --build . --config Release -j

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Set-Location ..

Write-Host ""
Write-Host "===================================" -ForegroundColor Green
Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "===================================" -ForegroundColor Green
Write-Host ""
Write-Host "To run the examples:" -ForegroundColor Cyan
Write-Host '  $env:OPENAI_API_KEY="your-api-key"' -ForegroundColor White
Write-Host "  .\build\Release\openai_asio.exe                # Sync example" -ForegroundColor White
Write-Host "  .\build\Release\openai_asio.exe --async        # Async example" -ForegroundColor White
Write-Host "  .\build\Release\openai_asio.exe --http-test    # HTTP client test" -ForegroundColor White
Write-Host ""

