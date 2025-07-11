if(NOT TARGET glm::glm)
    get_filename_component(GLM_SOURCE_DIR "${CMAKE_SOURCE_DIR}/libs/glm" ABSOLUTE)
    add_subdirectory("${GLM_SOURCE_DIR}" GLM_build)
endif()

if(TARGET glm)
    add_library(glm::glm ALIAS glm)
endif()