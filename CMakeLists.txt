cmake_minimum_required(VERSION 3.15)
project(A_completely_normal_fishing_game VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Packages
find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED)
find_package(Jolt REQUIRED)
find_package(glm REQUIRED)
find_package(glew REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(PkgConfig REQUIRED)

#Add subdirectories for your engine and game logic
add_subdirectory(src/engine)
add_subdirectory(src/game)
add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2              # SDL2 linking
    SDL2_image::SDL2_image  # SDL images
    OpenGL::GL              # Linking OpenGL
    GLEW::GLEW              # GLEW linking
    boost::boost            # Boost linking
    Jolt::Jolt              # Jolt linking
    SQLite::SQLite3         # Sqlite linking
    engine
    game
)

# Post-build asset copy
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "---- Copying assets to output directory ----"
)