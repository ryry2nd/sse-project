include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

add_custom_target(copy_glm_headers ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${glm_SOURCE_DIR}/glm
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/glm
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${glm_SOURCE_DIR}/copying.txt
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/glm
        COMMENT "---- Copying glm headers ----"
)