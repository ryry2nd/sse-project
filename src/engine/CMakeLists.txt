file(GLOB SCRIPTING_HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Scripting/*.hpp
)

file(GLOB RENDERING_HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/*.hpp
)


file(GLOB MODULE_STD_HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Scripting/Standard/*.hpp
)

set(HEADER_STAMP ${CMAKE_BINARY_DIR}/include/.headers_ok)

add_custom_command(
    OUTPUT ${HEADER_STAMP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include

    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/Rendering/engine/Rendering
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/Scripting/engine/Scripting
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/Engine/engine

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${RENDERING_HEADERS}
        ${CMAKE_BINARY_DIR}/include/Rendering/engine/Rendering

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SCRIPTING_HEADERS}
        ${CMAKE_BINARY_DIR}/include/Scripting/engine/Scripting

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MODULE_STD_HEADERS}
        ${CMAKE_BINARY_DIR}/include/Engine/engine

    COMMAND ${CMAKE_COMMAND} -E touch ${HEADER_STAMP}

    DEPENDS
        ${MATH_HEADERS}
        ${BIG_OBJECT_HEADERS}
        ${RENDERING_HEADERS}
        ${SCRIPTING_HEADERS}
        ${MODULE_STD_HEADERS}

    COMMENT "---- Copying headers ----"
)

add_custom_target(copy_src_headers DEPENDS ${HEADER_STAMP})

#Rendering Section

file(GLOB rendering_SOURCES "Rendering/*.cpp")

add_library(RenderingBase SHARED ${rendering_SOURCES})
add_dependencies(RenderingBase copy_src_headers)
target_include_directories(RenderingBase PUBLIC
	${CMAKE_BINARY_DIR}/include/Rendering
)
set_target_properties(RenderingBase PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/lib"
)

target_link_libraries(RenderingBase PRIVATE PNG::PNG PUBLIC SDL3::SDL3 glm::glm spdlog::spdlog)


if(USE_OPENGL)
	file(GLOB GL_SOURCES "Rendering/opengl/*.cpp")
	add_library(RenderingOpenGL SHARED ${GL_SOURCES})
	add_dependencies(RenderingOpenGL copy_src_headers)
	set_target_properties(RenderingOpenGL PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Rendering)
	target_include_directories(RenderingOpenGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/opengl)
	target_link_libraries(RenderingOpenGL PRIVATE RenderingBase)
	target_link_libraries(RenderingOpenGL PRIVATE SDL3::SDL3 glm::glm OPENGL::OPENGL spdlog::spdlog)
	set_target_properties(RenderingOpenGL PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/lib"
	)
endif()


# Scripting Section
file(GLOB scriptingJIT_SOURCES "Scripting/JIT/*.cpp")

add_library(ScriptingJIT SHARED ${scriptingJIT_SOURCES})
add_dependencies(ScriptingJIT copy_src_headers)

set_target_properties(ScriptingJIT PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/lib"
)

target_link_libraries(ScriptingJIT PRIVATE SDL3::SDL3 spdlog::spdlog RenderingBase yaml-cpp::yaml-cpp)

if (SSE_MINIMAL)
target_compile_definitions(ScriptingJIT PRIVATE MINIMAL)
else()
target_link_libraries(ScriptingJIT PRIVATE CLANG::CLANG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
add_custom_command(
	TARGET ScriptingJIT POST_BUILD
	COMMAND ${CMAKE_STRIP} $<TARGET_FILE:ScriptingJIT>
	COMMENT "---- Stripping JIT ----"
)
endif()


file(GLOB scripting_SOURCES "Scripting/*.cpp")

add_library(Scripting SHARED ${scripting_SOURCES})
add_dependencies(Scripting copy_src_headers)
target_include_directories(Scripting PUBLIC
	${CMAKE_BINARY_DIR}/include/Scripting)

set_target_properties(Scripting PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/lib"
)

target_link_libraries(Scripting PRIVATE ScriptingJIT yaml-cpp::yaml-cpp PUBLIC glm::glm SDL3::SDL3 spdlog::spdlog)


add_library(EngineABI INTERFACE)

target_include_directories(EngineABI INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Engine
)