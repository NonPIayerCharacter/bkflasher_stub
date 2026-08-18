#!/bin/bash
mkdir -p build
mkdir -p build/all
for dir in platforms/*/; do
    platform=${dir%*/}
    platform=${platform##*/}
    make PLATFORM="$platform"
    cp build/$platform/${platform}_Stub.bin build/all/${platform}_Stub.bin
    rm -rf build/${platform}
    make PLATFORM="$platform" -j$(nproc) CLIFLAGS="-DNO_MINIZ_COMPRESSION -DNO_MINIZ_DECOMPRESSION -DDISABLE_KV"
    cp build/$platform/${platform}_Stub.bin build/all/${platform}_Stub_minimal.bin
    rm -rf build/${platform}
done
