#!/usr/bin/env bash

set -e

export PATH="$HOME/.local/bin:$PATH"

TOOLCHAIN_FILE="cmake/toolchains/${OS}.cmake"

if [ "$MINIMAL" -eq 1 ]; then
BUILD_DIR="./build/$OS-$BUILD_TYPE-minimal"
else
BUILD_DIR="./build/$OS-$BUILD_TYPE"
fi

mkdir -p "$BUILD_DIR"

if [ "$NOCONFIG" -eq 0 ]; then
  cmake -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE -DSSE_MINIMAL=$MINIMAL
fi

cmake --build "$BUILD_DIR" -- -j"$CORES"