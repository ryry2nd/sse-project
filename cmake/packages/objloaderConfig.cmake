include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

FetchContent_Declare(
  tinyobjloader
  GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
  GIT_TAG        v2.0.0rc13
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(tinyobjloader)

add_library(OBJ::OBJ ALIAS tinyobjloader)