#!/bin/bash
export PATH="$HOME/.local/bin:$PATH"

BUILD_TYPE="Release"
OS="linux"
CORES=$(nproc)
useCMDClang="OFF"
NOCONFIG=0

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
	--os)
	  OS="$2"
	  shift 2
	  ;;
	--cores)
	  CORES="$2"
	  shift 2
	  ;;
	--useCMDClang)
	  useCMDClang="ON"
	  shift
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

if [ "$NOCONFIG" -eq 0 ]; then
  cmake -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE -DUSECMDCLANG=$useCMDClang
fi

cmake --build "$BUILD_DIR" -- -j"$CORES"