# bootstrap-qwindowkit.ps1

# Root working directory
$CWD = Split-Path -Parent $MyInvocation.MyCommand.Path
$SourceDir = Join-Path $CWD "vendor\src\QWindowKit"
$BuildDir = Join-Path $CWD "vendor\build\QWindowKit"
$InstallDir = Join-Path $CWD "vendor\bin\QWindowKit"

# Ensure submodules are initialized and updated
Write-Host "Initializing and updating submodules..."
git submodule update --init --recursive

# Ensure build and install directories exist
New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null

# Configure the project
cmake -S $SourceDir `
      -B $BuildDir `
      -G "Ninja Multi-Config" `
      -DCMAKE_PREFIX_PATH="C:\Qt\6.9.1\msvc2022_64" `
      -DQWINDOWKIT_BUILD_STATIC=ON `
      -DQWINDOWKIT_BUILD_WIDGETS=ON `
      -DCMAKE_INSTALL_PREFIX="$InstallDir"

# Build and install Debug
cmake --build $BuildDir --target install --config Debug

# Build and install Release
cmake --build $BuildDir --target install --config Release

Write-Host "QWindowKit built and installed to $InstallDir"
