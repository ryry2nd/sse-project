cmake_minimum_required(VERSION 3.15)

# SSE is a Small Engine
project(sse-project VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(USE_VULKAN "Enable Vulkan support" OFF)
option(USE_OPENGL "Enable OpenGL support" OFF)

add_compile_options(
	-Wdeprecated-literal-operator
	-Wdelete-abstract-non-virtual-dtor
	-Winconsistent-missing-override
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	message(STATUS "Configuring Release build: stripping debug info")
	add_compile_options(
		-O2
		-flto
		-fdata-sections
		-ffunction-sections
		-fno-omit-frame-pointer
	)

	add_link_options(
		-flto
    	-Wl,--gc-sections,-s
	)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "Configuring Debug build: fast compile")
	add_compile_options(
		-DDEBUG
		-O1
		-g3
		-fno-omit-frame-pointer
	)

	add_link_options(
		# keep linking simple/fast
	)
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/packages")

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/lib)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/lib)
endif()



# Packages
find_package(sdl CONFIG REQUIRED)
find_package(sdl_image CONFIG REQUIRED)
find_package(sdl_ttf CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
# find_package(boost_multiprecision CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_package(slang CONFIG REQUIRED)
find_package(compiler CONFIG REQUIRED)

# Add subdirectories
add_subdirectory(src/engine)
add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE
	RenderingBase
	Scripting
)

add_custom_target(delete_cache ALL
	COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/cache"
	COMMENT "---- Deleting folder recursively ----"
)

#compile modules
file(GLOB MODULE_DIRS LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/*"
)

foreach(MODULE ${MODULE_DIRS})
    if(IS_DIRECTORY ${MODULE})
		if (NOT EXISTS "${MODULE}/.ignore")
			get_filename_component(MODULE_NAME ${MODULE} NAME)
			set(MODULE_OUT_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/modules/${MODULE_NAME}")
			if(EXISTS "${MODULE}/CMakeLists.txt")
				add_subdirectory(${MODULE})
			endif()
			if(EXISTS "${MODULE}/assets")
			add_custom_target(copy_assets ALL
				COMMAND ${CMAKE_COMMAND} -E copy_directory
					${MODULE}/assets
					${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/modules/${MODULE_NAME}/assets
				COMMENT "---- Copying assets ----"
			)
			endif()
			if(EXISTS "${MODULE}/config.yaml")
			add_custom_target(copy_config ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
					${MODULE}/config.yaml
					${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/modules/${MODULE_NAME}
				COMMENT "---- Copying assets ----"
			)
			endif()
			message(STATUS "Module: ${MODULE_NAME}")
		endif()
    endif()
endforeach()

add_custom_target(copy_compile_command ALL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
		${CMAKE_CURRENT_BINARY_DIR}/..
	DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
	COMMENT "---- Copying compile commands ----"
)