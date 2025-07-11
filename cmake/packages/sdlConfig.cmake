set(SDL_TEST OFF)
set(SDL_VENDORED TRUE CACHE BOOL "Use vendored dependencies" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/sdl" SDL_BUILD)

if(TARGET SDL3)
    add_library(SDL3::SDL3 ALIAS SDL3)
elseif(TARGET SDL3-static)
    add_library(SDL3::SDL3 ALIAS SDL3-static)
endif()