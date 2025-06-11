conan install . --build=missing -c tools.system.package_manager:sudo=True -c tools.system.package_manager:mode=install -s build_type=Debug -g CMakeToolchain -g CMakeDeps &&
cmake --preset conan-debug &&
cmake --build --preset conan-debug