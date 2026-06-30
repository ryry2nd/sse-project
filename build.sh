#!/usr/bin/env bash

set -e

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

mkdir -p build .ccache

docker run --rm \
	-u $(id -u):$(id -g) \
	-e HOME=/tmp \
	-v "$(pwd)/src:$(pwd)/src:ro" \
	-v "$(pwd)/docker:$(pwd)/docker:ro" \
	-v "$(pwd)/cmake:$(pwd)/cmake:ro" \
	-v "$(pwd)/CMakeLists.txt:$(pwd)/CMakeLists.txt:ro" \
	-v "$(pwd)/build:$(pwd)/build" \
	-v "$(pwd)/.ccache:$(pwd)/.ccache" \
	-e BUILD_TYPE=${BUILD_TYPE} \
	-e OS=${OS} \
	-e CORES=${CORES} \
	-e NOCONFIG=${NOCONFIG} \
	-e MINIMAL=${MINIMAL} \
	-e CCACHE_DIR=$(pwd)/.ccache \
	--workdir $(pwd) \
	sse-builder

if [ "$MINIMAL" -eq 1 ]; then
BUILD_DIR="$OS-$BUILD_TYPE-minimal"
else
BUILD_DIR="$OS-$BUILD_TYPE"
fi

cp build/${BUILD_DIR}/compile_commands.json build/compile_commands.json