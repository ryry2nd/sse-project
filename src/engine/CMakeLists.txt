add_library(CustomMath INTERFACE)
target_include_directories(CustomMath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CustomMath)


file(GLOB_RECURSE base_SOURCES "Rendering/base/*.cpp")
add_library(RenderingBase SHARED ${base_SOURCES})
target_include_directories(RenderingBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/base)

if(USE_OPENGL)
    file(GLOB_RECURSE GL_SOURCES "Rendering/opengl/*.cpp")
    add_library(RenderingOpenGL SHARED ${GL_SOURCES})
    set_target_properties(RenderingOpenGL PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Rendering)
    target_include_directories(RenderingOpenGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/opengl)
    target_link_libraries(RenderingOpenGL PRIVATE RenderingBase)
endif()


file(GLOB_RECURSE object_SOURCES "BigObjects/*.cpp")
add_library(BigObjects SHARED ${object_SOURCES})
target_include_directories(BigObjects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/BigObjects)
target_link_libraries(BigObjects PRIVATE RenderingBase CustomMath)


file(GLOB_RECURSE scripting_SOURCES "Scripting/*.cpp")
add_library(Scripting SHARED ${scripting_SOURCES})
target_include_directories(Scripting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Scripting)