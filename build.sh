#!/bin/bash
export PATH="$HOME/.local/bin:$PATH"

BUILD_TYPE="Release"
OS="linux"
CORES=$(nproc)
NOCONFIG=0
MINIMAL=0

while [[ $# -gt 0 ]]; do
  case $1 in
	--debug)
	  BUILD_TYPE="Debug"
	  shift
	  ;;
	--no-config)
	  NOCONFIG=1
	  shift
	  ;;
	--minimal)
	  MINIMAL=1
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