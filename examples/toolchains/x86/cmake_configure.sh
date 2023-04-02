#!/bin/sh
cmake . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./toolchains/x86/linux-gnu-toolchain.cmake