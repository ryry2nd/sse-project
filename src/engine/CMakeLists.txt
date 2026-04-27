file(GLOB_RECURSE MATH_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/CustomMath/*.hpp
)
file(GLOB_RECURSE BIG_OBJECT_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/BigObjects/*.hpp
)
file(GLOB_RECURSE SCRIPTING_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/Scripting/*.hpp
)
file(GLOB_RECURSE RENDERING_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/*.hpp
)
list(FILTER RENDERING_HEADERS INCLUDE REGEX ".*/Rendering/[^/]+$")

add_custom_target(copy_src_headers ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/CustomMath
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/CustomMath/engine
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/CustomMath/engine/CustomMath
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/BigObjects
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/BigObjects/engine
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/BigObjects/engine/BigObjects
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/Rendering
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/Rendering/engine
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/Rendering/engine/Rendering
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/Scripting
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/Scripting/engine
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/include/Scripting/engine/Scripting
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${MATH_HEADERS}
                ${CMAKE_CURRENT_BINARY_DIR}/include/CustomMath/engine/CustomMath
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${BIG_OBJECT_HEADERS}
                ${CMAKE_CURRENT_BINARY_DIR}/include/BigObjects/engine/BigObjects
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${RENDERING_HEADERS}
                ${CMAKE_CURRENT_BINARY_DIR}/include/Rendering/engine/Rendering
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SCRIPTING_HEADERS}
                ${CMAKE_CURRENT_BINARY_DIR}/include/Scripting/engine/Scripting
        COMMENT "---- Copying headers ----"
)



#Custom Math section
add_library(CustomMath INTERFACE)
add_dependencies(CustomMath copy_src_headers)
target_include_directories(CustomMath INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/include/CustomMath
)
target_link_libraries(CustomMath INTERFACE glm::glm Boost::multiprecision spdlog::spdlog)



#Rendering Section

file(GLOB_RECURSE rendering_SOURCES "Rendering/*.cpp")
list(FILTER rendering_SOURCES INCLUDE REGEX ".*/Rendering/[^/]+$")

add_library(RenderingBase SHARED ${rendering_SOURCES})
add_dependencies(RenderingBase copy_src_headers)
target_include_directories(RenderingBase PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/include/Rendering
)

target_link_libraries(RenderingBase PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image glm::glm spdlog::spdlog)


if(USE_OPENGL)
    file(GLOB_RECURSE GL_SOURCES "Rendering/opengl/*.cpp")
    add_library(RenderingOpenGL SHARED ${GL_SOURCES})
    add_dependencies(RenderingOpenGL copy_src_headers)
    set_target_properties(RenderingOpenGL PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Rendering)
    target_include_directories(RenderingOpenGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/opengl)
    target_link_libraries(RenderingOpenGL PRIVATE RenderingBase)
    target_link_libraries(RenderingOpenGL PRIVATE SDL3::SDL3 glm::glm OPENGL::OPENGL spdlog::spdlog)
endif()



# #BigObjects Section

# file(GLOB_RECURSE object_SOURCES "BigObjects/*.cpp")

# add_library(BigObjects SHARED ${object_SOURCES})
# add_dependencies(BigObjects copy_src_headers)
# target_include_directories(BigObjects PUBLIC
#     ${CMAKE_CURRENT_BINARY_DIR}/include/BigObjects
# )
# target_link_libraries(BigObjects INTERFACE glm::glm spdlog::spdlog)

# target_link_libraries(BigObjects PRIVATE RenderingBase CustomMath)



# Scripting Section

file(GLOB_RECURSE scripting_SOURCES "Scripting/*.cpp")

add_library(Scripting SHARED ${scripting_SOURCES})
add_dependencies(Scripting copy_src_headers)
target_include_directories(Scripting PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/include/Scripting)

target_link_libraries(Scripting PRIVATE glm::glm yaml-cpp::yaml-cpp SDL3::SDL3 spdlog::spdlog)

target_compile_definitions(Scripting PRIVATE
    USECMDCLANG=$<BOOL:${USECMDCLANG}>
)