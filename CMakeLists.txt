cmake_minimum_required(VERSION 3.15)
project(A_completely_normal_fishing_game VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/packages")

# Packages
find_package(sdl REQUIRED)
find_package(sdl_image REQUIRED)
find_package(sdl_ttf REQUIRED)
find_package(glm REQUIRED)
find_package(boost_multiprecision REQUIRED)
find_package(sol2 REQUIRED)
find_package(yaml-cpp REQUIRED)

#conan
find_package(glew REQUIRED)
# find_package(Jolt REQUIRED)
# find_package(SQLite3 REQUIRED)
# find_package(sol2 REQUIRED)
# find_package(assimp REQUIRED)
# find_package(miniaudio REQUIRED)
# find_package(Angelscript REQUIRED)
# find_package(enet REQUIRED)
# find_package(glslang REQUIRED)
# find_package(gettext REQUIRED)


add_library(common_libs INTERFACE)
target_link_libraries(common_libs INTERFACE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_image::SDL3_image
    glm::glm
    Boost::multiprecision
    sol2::sol2
    yaml-cpp::yaml-cpp

# conan
    GLEW::GLEW
    # Jolt::Jolt
    # SQLite::SQLite
    # assimp::assimp
    # Angelscript::angelscript
    # miniaudio::miniaudio
    # enet::enet
    # glslang::_glslang-do-not-use
    # gettext::gettext
)

#Add subdirectories for your engine and game logic
add_subdirectory(src/engine)
add_executable(${PROJECT_NAME} src/main.cpp)

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