#!/bin/bash
export PATH="$HOME/.local/bin:$PATH"

BUILD_TYPE="Release"
OS="linux"
CORES=$(nproc)

while [[ $# -gt 0 ]]; do
  case $1 in
    --debug)
      BUILD_TYPE="Debug"
      shift
      ;;
    --os)
      OS="$2"
      shift 2
      ;;
    --cores)
      CORES="$2"
      shift 2
      ;;
    *)
      echo "Unknown option $1"
      exit 1
      ;;
  esac
done

BUILD_DIR="./build/$OS-$BUILD_TYPE"
TOOLCHAIN_FILE="cmake/toolchains/${OS}.cmake"

mkdir -p "$BUILD_DIR"
cmake -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE

cmake --build "$BUILD_DIR" -- -j"$CORES"