file(GLOB VK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(RenderingVK SHARED ${VK_SOURCES})
add_dependencies(RenderingVK copy_src_headers)
set_target_properties(RenderingVK PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Rendering)
target_link_libraries(RenderingVK PRIVATE RenderingBase)
target_link_libraries(RenderingVK PRIVATE SDL3::SDL3 glm::glm OPENGL::OPENGL spdlog::spdlog VULKAN::VULKAN)
set_target_properties(RenderingVK PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/lib"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_link_libraries(RenderingVK PUBLIC TracyClient)
endif()