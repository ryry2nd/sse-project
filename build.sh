PROFILE_PATH=${1:-"conan/profiles/linux_x86_64_gcc"}

conan install . --build=missing -c tools.system.package_manager:sudo=True -c tools.system.package_manager:mode=install -s build_type=Release --profile "$PROFILE_PATH" -g CMakeToolchain -g CMakeDeps &&
cmake --preset conan-release &&
cmake --build --preset conan-release