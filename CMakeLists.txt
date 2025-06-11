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
find_package(sol2 REQUIRED)

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
add_subdirectory(src/game)
add_executable(${PROJECT_NAME} src/main.cpp)

target_compile_definitions(common_libs INTERFACE GLM_ENABLE_EXPERIMENTAL)

target_link_libraries(${PROJECT_NAME}
    common_libs
    engine
    game
)

target_link_libraries(engine PRIVATE common_libs)
target_link_libraries(game PRIVATE common_libs)

# Post-build asset copy
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "---- Copying assets to output directory ----"
)