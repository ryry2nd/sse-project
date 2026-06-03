file(GLOB GL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(RenderingOpenGL SHARED ${GL_SOURCES})
add_dependencies(RenderingOpenGL copy_src_headers)
set_target_properties(RenderingOpenGL PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Rendering)
target_link_libraries(RenderingOpenGL PRIVATE RenderingBase)
target_link_libraries(RenderingOpenGL PRIVATE SDL3::SDL3 glm::glm OPENGL::OPENGL spdlog::spdlog)
set_target_properties(RenderingOpenGL PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/lib"
)