add_library(customMath INTERFACE)
target_include_directories(customMath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/customMath)


file(GLOB_RECURSE base_SOURCES "rendering/base/HelperFunctions.cpp")
add_library(renderingBase SHARED ${base_SOURCES})
target_include_directories(renderingBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rendering/base)

if(USE_OPENGL)
    file(GLOB_RECURSE GL_SOURCES "rendering/opengl/*.cpp")
    add_library(renderingOpenGL SHARED ${GL_SOURCES})
    target_include_directories(renderingOpenGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rendering/opengl)
    target_link_libraries(renderingOpenGL PRIVATE renderingBase)
endif()


file(GLOB_RECURSE object_SOURCES "objects/*.cpp")
add_library(objects SHARED ${object_SOURCES})
target_include_directories(objects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/objects)
target_link_libraries(objects PRIVATE renderingBase customMath)


file(GLOB_RECURSE scripting_SOURCES "scripting/*.cpp")
add_library(scripting SHARED ${scripting_SOURCES})
target_include_directories(scripting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/scripting)