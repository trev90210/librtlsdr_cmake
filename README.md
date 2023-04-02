# Introduction
![x86-windows](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/x86-windows.yml/badge.svg)](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/x86-windows.yml)
[![x86-linux](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/x86-linux.yml/badge.svg)](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/x86-linux.yml)
[![arm-linux](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/arm-linux.yml/badge.svg)](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/arm-linux.yml)

This is a config.cmake for the rtlsdr library.

The repository this was compiled from is [here](https://github.com/steve-m/librtlsdr).

Build targets are:
- host=x64 target=x64

# Build environment
Built using MSYS2 Mingw64
- gcc = 12.2.0 (Rev6, Build by MSYS2 project)
- cmake = 3.24.3

# Instructions
### 1. Clone repository [here](https://github.com/steve-m/librtlsdr.git).

### 2. Install libusb-1.0 and winpthread
<code>pacman -Su mingw-w64-x86_64-libwinpthread-git</code>

<code>pacman -Su mingw-w64-x86_64-libusb</code>

### 3. Run cmake configuration 
<code>cmake . -B build -G Ninja</code>

### 4. Build with ninja 
<code>ninja -C build -j 4</code>

### 5. Copy librtlsdr.dll from build/src, and libusb-1.0.dll and libwinpthread-1.dll from the Mingw64 folders.

# Example code
Sample program is shown in examples/