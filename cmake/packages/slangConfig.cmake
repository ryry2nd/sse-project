#flags
set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
set(SLANG_ENABLE_SLANG_RHI ON)
set(SLANG_ENABLE_SLANGC OFF)
set(SLANG_ENABLE_SLANGD OFF)
set(SLANG_ENABLE_EXAMPLES OFF)
set(SLANG_ENABLE_TESTS OFF)
set(SLANG_USE_SYSTEM_MINIZ OFF)
set(SLANG_USE_SYSTEM_LZ4 OFF)
set(SLANG_USE_SYSTEM_GLSLANG OFF)
set(SLANG_LIB_TYPE STATIC)

#include proper functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/libs/slang/cmake")
include(AutoOption)
include(Glob)
include(CompilerFlags)
include(SlangTarget)

#build libraries
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/spirv-headers" slang_build_dir/external/spirv-headers)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/spirv-tools" slang_build_dir/external/spirv-tools)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/glslang" slang_build_dir/external/glslang)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/unordered_dense" slang_build_dir/external/unordered_dense)
# add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/lz4" slang_build_dir/external/lz4)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/miniz" slang_build_dir/external/miniz)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/slang-rhi" slang_build_dir/external/slang-rhi)

add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/vulkan" slang_build_dir/external/vulkan)
# add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/dxc" slang_build_dir/external/dxc)
# add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/external/metal-cpp" slang_build_dir/external/metal-cpp)

#build required slang modules
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/source/core" slang_build_dir/core)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/source/slang-rt" slang_build_dir/slang-rt)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/source/compiler-core" slang_build_dir/compiler-core)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/source/slang" slang_build_dir/sl)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/source/slang-glslang" slang_build_dir/slang-glslang)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/source/slang-glsl-module" slang_build_dir/slang-glsl-module)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/slang/source/slang-rhi" slang_build_dir/slang-rhi)

#make it an alias
add_library(SLANG::SLANG ALIAS slang)