# do I look like I know what a jpeg is, I'm just trying to print out a picture of a god dang hot dog.
include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)

set(PNG_INSTALL OFF CACHE BOOL "" FORCE)
set(PNG_TOOLS OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(PNG_FRAMEWORK OFF CACHE BOOL "" FORCE)
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_STATIC ON CACHE BOOL "" FORCE)

FetchContent_Declare(
	libpng
	GIT_REPOSITORY https://github.com/pnggroup/libpng.git
	GIT_TAG v1.6.58
)
FetchContent_MakeAvailable(libpng)

if(TARGET png_static)
    add_library(PNG::PNG ALIAS png_static)
elseif(TARGET png)
    add_library(PNG::PNG ALIAS png)
endif()



include(ExternalProject)

set(JPEG_PREFIX ${CMAKE_BINARY_DIR}/thirdparty/libjpeg)

ExternalProject_Add(libjpeg_turbo
    GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
    GIT_TAG 3.1.4

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${JPEG_PREFIX}
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DENABLE_SHARED=OFF
        -DENABLE_STATIC=ON
        -DWITH_JPEG8=ON
        -DWITH_SIMD=ON

    BUILD_BYPRODUCTS
        ${JPEG_PREFIX}/lib/libturbojpeg.a
        ${JPEG_PREFIX}/lib/libjpeg.a

    INSTALL_DIR
        ${JPEG_PREFIX}
)

# prevent configure-time include errors
file(MAKE_DIRECTORY ${JPEG_PREFIX}/include)

add_library(JPEG::JPEG STATIC IMPORTED GLOBAL)

set_target_properties(JPEG::JPEG PROPERTIES
    IMPORTED_LOCATION "${JPEG_PREFIX}/lib/libturbojpeg.a"
    INTERFACE_INCLUDE_DIRECTORIES "${JPEG_PREFIX}/include"
)

add_dependencies(JPEG::JPEG libjpeg_turbo)