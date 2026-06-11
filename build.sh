#!/usr/bin/env bash

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

set -e

mkdir -p build .ccache

docker run --rm \
	-u $(id -u):$(id -g) \
	-e HOME=/tmp \
	-v "$(pwd)/src:/src" \
	-v "$(pwd)/build:/build" \
	-v "$(pwd)/docker:/docker" \
	-v "$(pwd)/cmake:/cmake" \
	-v "$(pwd)/.ccache:/ccache" \
	-v "$(pwd)/CMakeLists.txt:/CMakeLists.txt" \
	-e BUILD_TYPE=${BUILD_TYPE} \
	-e OS=${OS} \
	-e CORES=${CORES} \
	-e NOCONFIG=${NOCONFIG} \
	-e MINIMAL=${MINIMAL} \
	-e CCACHE_DIR=/ccache \
	sse-builder