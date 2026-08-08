#!/bin/bash

for dir in platforms/*/; do
    platform=${dir%*/}
    platform=${platform##*/}
    make PLATFORM="$platform" -j$(nproc)
done
