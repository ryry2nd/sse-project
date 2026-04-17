set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glm" GLM_build)
target_compile_options(glm INTERFACE -w)
add_library(glm::glm ALIAS glm)

target_compile_definitions(glm INTERFACE GLM_ENABLE_EXPERIMENTAL)