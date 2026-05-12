include(FetchContent)

set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)
set(SDL_TEST_LIBRARY OFF)
set(SDL_VENDORED TRUE)
set(SDL_RENDER OFF)

FetchContent_Declare(
	sdl
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.4.8
)
FetchContent_MakeAvailable(sdl)