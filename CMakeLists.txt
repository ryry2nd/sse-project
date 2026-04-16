cmake_minimum_required(VERSION 3.15)

# SSE is a Small Engine
project(sse-project VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_VULKAN "Enable Vulkan support" OFF)
option(USE_OPENGL "Enable OpenGL support" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring Release build: stripping debug info")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -g0")
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/packages")

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/bin)

# Packages
find_package(sdl REQUIRED)
find_package(sdl_image REQUIRED)
find_package(sdl_ttf REQUIRED)
find_package(glm REQUIRED)
find_package(boost_multiprecision REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(glad REQUIRED)
find_package(slang REQUIRED)
# to be added llvm-clang

# Add subdirectories for your engine and game logic
add_subdirectory(src/engine)
add_executable(main src/main.cpp)

target_link_libraries(main
    SDL3::SDL3
    CustomMath
    RenderingBase
    BigObjects
    Scripting
)

target_link_libraries(RenderingBase PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image glm::glm)

if(USE_OPENGL)
    target_link_libraries(RenderingOpenGL PRIVATE SDL3::SDL3 glm::glm OPENGL::OPENGL)
endif()

target_link_libraries(BigObjects INTERFACE glm::glm)
target_link_libraries(CustomMath INTERFACE glm::glm Boost::multiprecision)
target_link_libraries(Scripting PRIVATE glm::glm yaml-cpp::yaml-cpp SDL3::SDL3)

add_custom_target(delete_cache ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/cache"
    COMMENT "---- Deleting folder recursively ----"
)

# Post-build asset copy
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/modules
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/modules
    COMMENT "---- Copying assets ----"
)

# copy headers
file(GLOB_RECURSE MATH_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine/CustomMath/*.hpp
)
file(GLOB_RECURSE BIG_OBJECT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine/BigObjects/*.hpp
)
file(GLOB_RECURSE RENDERING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine/Rendering/base/*.hpp
)

add_custom_target(copy_src_headers ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/CustomMath
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/BigObjects
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/Rendering
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/Rendering/base
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MATH_HEADERS}
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/CustomMath
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${BIG_OBJECT_HEADERS}
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/BigObjects
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${RENDERING_HEADERS}
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/Rendering/base
    COMMENT "---- Copying headers ----"
)
# copy compiler
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/compilers AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/compilers/clang AND NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/clang)
  add_custom_target(copy_compiler ALL
      COMMAND ${CMAKE_COMMAND} -E copy_directory
              ${CMAKE_CURRENT_SOURCE_DIR}/compilers/clang
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/clang
      COMMENT "---- Copying compiler ----"
  )
endif()

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/glm)
        add_custom_target(copy_lib_headers ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/libs/glm/glm
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/glm
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/libs/glm/copying.txt
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/glm
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/libs/multiprecision/include/boost
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/boost
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/libs/multiprecision/LICENSE
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/boost
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/libs/boost_config/include/boost
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/boost
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/libs/boost_config/build.jam
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/boost
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/libs/sdl/include
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/
        COMMENT "---- Copying Library headers ----"
        )
endif()