include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
set(SDLTTF_VENDORED TRUE)

FetchContent_Declare(
  sdl-ttf
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
  GIT_TAG release-3.2.2
)
FetchContent_MakeAvailable(sdl-ttf)