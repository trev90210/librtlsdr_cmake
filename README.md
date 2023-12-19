# Introduction
[![x86-windows](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/x86-windows.yml/badge.svg)](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/x86-windows.yml)
[![x86-linux](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/x86-linux.yml/badge.svg)](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/x86-linux.yml)
[![arm-linux](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/arm-linux.yml/badge.svg)](https://github.com/FiendChain/librtlsdr_cmake/actions/workflows/arm-linux.yml)

This is a config.cmake for the rtlsdr library. Compiled from [librtlsdr/librtlsdr](https://github.com/librtlsdr/librtlsdr/tree/fe225869acbe558fb014d858fb1bdd4dce96eab0).

# Build environment
Built in wsl using the mingw-w32 and mingw-w64 cross compilers.

Refer to the [github workflow here](https://github.com/librtlsdr/librtlsdr/blob/fe225869acbe558fb014d858fb1bdd4dce96eab0/.github/workflows/c-cpp.yml).
- Job: ```cross_build_win32_win64_latest``` to replicate cross compile on wsl.
- Steps: ```build_w32_dlldep``` and ```build_w64_dlldep``` for relevant commands.

# Example code
Sample program is shown in ```examples/```.