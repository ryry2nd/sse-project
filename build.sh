conan install . --build=missing -c tools.system.package_manager:sudo=True -c tools.system.package_manager:mode=install -s build_type=Release -g CMakeToolchain -g CMakeDeps &&
cmake --preset conan-release &&
cmake --build --preset conan-release