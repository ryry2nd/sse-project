#!/bin/bash
export PATH="$HOME/.local/bin:$PATH"

BUILD_TYPE="Release"
OS="linux"

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
    *)
      echo "Unknown option $1"
      exit 1
      ;;
  esac
done

# PROFILE_PATH=${1:-"conan/profiles/linux_x86_64_gcc"}

conan install . --build=missing -c tools.system.package_manager:sudo=True -c tools.system.package_manager:mode=install -s build_type="$BUILD_TYPE" --profile conan/profiles/linux_x86_64_gcc -g CMakeToolchain -g CMakeDeps

if ["$BUILD_TYPE" == "Release"]; then
    cmake --preset conan-release &&
    cmake --build --preset conan-release
else
    cmake --preset conan-debug &&
    cmake --build --preset conan-debug
fi

# BUILD_DIR="./build/$OS-$BUILD_TYPE"
# TOOLCHAIN_FILE="cmake/toolchains/${OS}.cmake"

# mkdir -p "$BUILD_DIR"
# cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE
# cmake --build "$BUILD_DIR"