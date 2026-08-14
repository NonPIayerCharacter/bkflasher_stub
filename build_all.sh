#!/bin/bash
mkdir -p build
mkdir -p build/all
for dir in platforms/*/; do
    platform=${dir%*/}
    platform=${platform##*/}
    make PLATFORM="$platform" -j$(nproc)
    cp build/$platform/${platform}_Stub.bin build/all/${platform}_Stub.bin
done
