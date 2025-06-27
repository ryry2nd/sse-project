cmake_minimum_required(VERSION 4.0.2)
project(A_completely_normal_fishing_game VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Packages
find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED)
find_package(Jolt REQUIRED)
find_package(glm REQUIRED)
find_package(glew REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(sol2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

if (NOT WIN32)
    find_package(PkgConfig REQUIRED)
endif()


add_library(common_libs INTERFACE)
target_link_libraries(common_libs INTERFACE
    SDL2::SDL2              # SDL2 linking
    SDL2_image::SDL2_image  # SDL images
    GLEW::GLEW              # GLEW linking
    boost::boost            # Boost linking
    Jolt::Jolt              # Jolt linking
    SQLite::SQLite3         # Sqlite linking
    sol2::sol2              # sol2 linking
    glm::glm                # glm linking
    sdl_ttf::sdl_ttf      # sdl ttf linking
)


if (WIN32)
    target_link_libraries(common_libs INTERFACE
        SDL2::SDL2main        # SDL2 main on Windows
        opengl32              # Windows OpenGL lib
    )
else()
    target_link_libraries(common_libs INTERFACE
        OpenGL::GL            # Unix-like OpenGL
    )
endif()

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
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMENT "---- Copying assets to output directory ----"
)

add_custom_target(copy_directory ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/game
            ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "---- Copying game to output directory ----"
)