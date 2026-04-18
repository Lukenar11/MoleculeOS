#!/usr/bin/env sh
ninja -f build/build.ninja
qemu-system-i386 -kernel build/bin/kernel.elf
