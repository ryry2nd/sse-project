cmake_minimum_required(VERSION 3.15)
project(A_completely_normal_fishing_game VERSION 0.0.1 LANGUAGES CXX)


message(STATUS "System Name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
if(WIN32)
  message(STATUS "Building for Windows")
elseif(ANDROID)
  message(STATUS "Building for Android")
elseif(EMSCRIPTEN)
  message(STATUS "Building for WebAssembly")
elseif(UNIX AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(PLATFORM "Building for Raspberry pi")
elseif(UNIX)
  message(STATUS "Building for Unix/Linux OS")
else()
  message(STATUS "Unknown OS")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/packages")


# Packages
find_package(sdl REQUIRED)
find_package(sdl_image REQUIRED)
find_package(sdl_ttf REQUIRED)
find_package(glm REQUIRED)

#conan
find_package(Boost REQUIRED)
# find_package(Jolt REQUIRED)
find_package(glew REQUIRED)
# find_package(SQLite3 REQUIRED)
find_package(sol2 REQUIRED)
find_package(yaml-cpp REQUIRED)
# find_package(assimp REQUIRED)
# find_package(miniaudio REQUIRED)
# find_package(Angelscript REQUIRED)
# find_package(enet REQUIRED)
# find_package(glslang REQUIRED)
# find_package(gettext REQUIRED)

# if (UNIX)
#     find_package(PkgConfig REQUIRED)
#     # find_package(SDL2_IMAGE REQUIRED)
#     find_package(OpenGL REQUIRED)
#     find_package(VulkanHeaders REQUIRED)
# elseif (WIN32)
#     # find_package(SDL2_image REQUIRED)
#     find_package(OpenGL REQUIRED)
#     find_package(VulkanHeaders REQUIRED)
# endif()


add_library(common_libs INTERFACE)
target_link_libraries(common_libs INTERFACE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_image::SDL3_image
    glm::glm

# conan
    GLEW::GLEW
    boost::boost
    # Jolt::Jolt
    # SQLite::SQLite
    sol2::sol2
    yaml-cpp::yaml-cpp
    # assimp::assimp
    # Angelscript::angelscript
    # miniaudio::miniaudio
    # enet::enet
    # glslang::_glslang-do-not-use
    # gettext::gettext
)

# if (WIN32)
#     target_link_libraries(common_libs INTERFACE
#         SDL2::SDL2main        # SDL2 main on Windows
#         opengl32              # Windows OpenGL lib
#         vulkan-headers::vulkan-headers  # vulkan headers linking
#     )
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows -Wl,--entry=mainCRTStartup -static -static-libgcc -static-libstdc++ -Wl,--subsystem,windows")

# elseif (UNIX)
#     target_link_libraries(common_libs INTERFACE
#         OpenGL::GL            # Unix-like OpenGL
#         vulkan-headers::vulkan-headers  # vulkan headers linking
#     )
# endif()

#Add subdirectories for your engine and game logic
add_subdirectory(src/engine)
add_executable(${PROJECT_NAME} src/main.cpp)

target_compile_definitions(common_libs INTERFACE GLM_ENABLE_EXPERIMENTAL)

target_link_libraries(${PROJECT_NAME}
    common_libs
    engine
)

target_link_libraries(engine PRIVATE common_libs)

# Post-build asset copy
add_custom_target(copy_directory ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/game
            ${CMAKE_CURRENT_BINARY_DIR}/game
    COMMENT "---- Copying game to output directory ----"
)