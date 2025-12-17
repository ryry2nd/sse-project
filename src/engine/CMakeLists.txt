add_library(customMath INTERFACE)

target_include_directories(customMath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/customMath)



file(GLOB_RECURSE object_SOURCES "objects/*.cpp")

add_library(objects SHARED ${object_SOURCES})
target_include_directories(objects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/objects)



file(GLOB_RECURSE base_SOURCES "rendering/base/HelperFunctions.cpp")
add_library(renderingBase SHARED ${base_SOURCES})
target_include_directories(renderingBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rendering/base)

if(USE_OPENGL)
    file(GLOB_RECURSE GL_SOURCES "rendering/opengl/*.cpp")
endif()

list(APPEND Rendering_SOURCES ${GL_SOURCES})# ${VK_SOURCES}
add_library(renderingAPI SHARED ${Rendering_SOURCES})

if(USE_OPENGL)
    target_include_directories(renderingAPI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rendering/opengl)
endif()



# file(GLOB_RECURSE scripting_SOURCES "scripting/*.cpp")

# add_library(scripting SHARED ${scripting_SOURCES})
# target_include_directories(scripting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/scripting)



target_link_libraries(objects PRIVATE renderingBase customMath)
target_link_libraries(renderingAPI PRIVATE renderingBase)
# target_link_libraries(scripting PRIVATE rendering objects)
# target_link_libraries(scripting INTERFACE customMath)