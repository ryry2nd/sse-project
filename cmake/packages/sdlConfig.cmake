include(FetchContent)

set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)
set(SDL_TEST_LIBRARY OFF)
set(SDL_VENDORED TRUE)

FetchContent_Declare(
  sdl
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.4.8
)
FetchContent_MakeAvailable(sdl)

add_custom_target(copy_sdl_headers ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${sdl_SOURCE_DIR}/include
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${sdl_SOURCE_DIR}/LICENSE.txt
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/SDL3
        COMMENT "---- Copying SDL headers ----"
)