#!/bin/bash
export PATH="$HOME/.local/bin:$PATH"

BUILD_TYPE="Release"
OS="linux"
CORES=0

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
      CORES="$3"
      shift 3
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
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE

# Determine number of cores to use
if [[ "$CORES" -eq 0 ]]; then
    CORES=$(nproc)
fi

cmake --build "$BUILD_DIR" -- -j"$CORES"