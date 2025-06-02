conan install . --build=missing -c tools.system.package_manager:mode=install --profile=conanprofileWindows -s build_type=Release -g CMakeToolchain -g CMakeDeps &&
mkdir build && cd build &&
cmake .. -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release &&
cmake --build .
