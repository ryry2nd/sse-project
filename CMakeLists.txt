cmake_minimum_required(VERSION 3.15)
project(A_completely_normal_fishing_game VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_VULKAN "Enable Vulkan support" OFF)
option(USE_OPENGL "Enable OpenGL support" OFF)
set(DEFAULT_API "OpenGL" CACHE STRING "Default graphics API")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring Release build: stripping debug info")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -g0")
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/packages")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# Packages
find_package(sdl REQUIRED)
find_package(sdl_image REQUIRED)
find_package(sdl_ttf REQUIRED)
find_package(glm REQUIRED)
find_package(boost_multiprecision REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(glad REQUIRED)
# to be added, jolt, sqlite3, assimp, miniaudio, enet, glslang, gettext, wasm

#Add subdirectories for your engine and game logic
add_subdirectory(src/engine)
add_executable(${PROJECT_NAME} src/main.cpp)

if(USE_VULKAN)
  target_compile_definitions(${PROJECT_NAME} PUBLIC USE_VULKAN)
endif()

if(USE_OPENGL)
  target_compile_definitions(${PROJECT_NAME} PUBLIC USE_OPENGL)
endif()

if(DEFAULT_API STREQUAL "OpenGL")
  set(DEFAULT_API_INT 1)
elseif(DEFAULT_API STREQUAL "Vulkan")
  set(DEFAULT_API_INT 2)
else()
  set(DEFAULT_API_INT 0)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC DEFAULT_API=${DEFAULT_API_INT})

target_link_libraries(${PROJECT_NAME}
    SDL3::SDL3
    customMath
    renderingBase
    renderingAPI
    objects
    # scripting
)

target_link_libraries(renderingBase PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image glm::glm)
target_link_libraries(renderingAPI PRIVATE SDL3::SDL3 glm::glm glad::glad)
target_link_libraries(objects INTERFACE glm::glm)
target_link_libraries(customMath INTERFACE glm::glm Boost::multiprecision)

# target_link_libraries(scripting PRIVATE common_libs)

# Post-build asset copy
add_custom_target(copy_directory ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/game
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/game
    COMMENT "---- Copying game to output directory ----"
)