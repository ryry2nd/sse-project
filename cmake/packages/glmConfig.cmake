set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glm" GLM_build)
add_library(glm::glm ALIAS glm)

target_compile_definitions(glm INTERFACE GLM_ENABLE_EXPERIMENTAL)