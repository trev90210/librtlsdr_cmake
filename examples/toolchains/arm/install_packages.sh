#!/bin/sh
sudo apt-get --yes install build-essential ninja-build
sudo apt-get --yes install qemu-user qemu-user-static gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu binutils-aarch64-linux-gnu-dbg
sudo dpkg --add-architecture arm64
sudo cat ./toolchains/arm/sources.list | sudo tee -a /etc/apt/sources.list
sudo apt-get --yes update
sudo apt-get --yes install librtlsdr-dev:arm64