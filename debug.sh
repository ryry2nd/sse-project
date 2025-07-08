PROFILE_PATH=${1:-"conan/profiles/linux_x86_64_gcc"}

conan install . --build=missing -c tools.system.package_manager:sudo=True -c tools.system.package_manager:mode=install --profile "$PROFILE_PATH" -s build_type=Debug -g CMakeToolchain -g CMakeDeps &&
cmake --preset conan-debug &&
cmake --build --preset conan-debug