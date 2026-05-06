# do I look like I know what a jpeg is, I'm just trying to print out a picture of a god dang hot dog.
include(FetchContent)

set(SDLIMAGE_XV OFF CACHE BOOL "Disable XV support" FORCE)
set(SDLIMAGE_ANI OFF CACHE BOOL "Disable ANI support" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "Disable TIFF support" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "Disable WebP support" FORCE)
set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable AVIF support" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "Disable JXL support" FORCE)
set(SDLIMAGE_QOI OFF CACHE BOOL "Disable QOI support" FORCE)
set(SDLIMAGE_GIF OFF CACHE BOOL "Disable GIF support" FORCE)
set(SDLIMAGE_BMP OFF CACHE BOOL "Disable BMP support" FORCE)
set(SDLIMAGE_XPM OFF CACHE BOOL "Disable XPM support" FORCE)
set(SDLIMAGE_XCF OFF CACHE BOOL "Disable XCF support" FORCE)
set(SDLIMAGE_LBM OFF CACHE BOOL "Disable LBM support" FORCE)
set(SDLIMAGE_PCX OFF CACHE BOOL "Disable PCX support" FORCE)
set(SDLIMAGE_PNM OFF CACHE BOOL "Disable PNM support" FORCE)
set(SDLIMAGE_SVG OFF CACHE BOOL "Disable SVG support" FORCE)
set(SDLIMAGE_TGA OFF CACHE BOOL "Disable TGA support" FORCE)
set(SDLIMAGE_MOD OFF CACHE BOOL "Disable MOD support" FORCE)
set(SDLIMAGE_OPUS OFF CACHE BOOL "Disable OPUS support" FORCE)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

FetchContent_Declare(
	sdl-image
	GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
	GIT_TAG release-3.4.4
)
FetchContent_MakeAvailable(sdl-image)