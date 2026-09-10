#!/bin/bash
mkdir -p build
mkdir -p build/all
mkdir -p build/all/minimal
for dir in platforms/*/; do
    platform=${dir%*/}
    platform=${platform##*/}
    make prebuild PLATFORM="$platform"
    make PLATFORM="$platform" -j$(nproc)
    cp build/$platform/${platform}_Stub.bin build/all/${platform}_Stub.bin
    rm -rf build/${platform}
    make PLATFORM="$platform" -j$(nproc) CLIFLAGS="-DNO_MINIZ_COMPRESSION -DNO_MINIZ_DECOMPRESSION -DDISABLE_KV"
    cp build/$platform/${platform}_Stub.bin build/all/minimal/${platform}_Stub.bin
    rm -rf build/${platform}
done
