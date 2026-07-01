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

if (SSE_MINIMAL)
	message(STATUS "Configuring Release build: stripping debug info")
	add_compile_options(
		-Oz
		-flto
		-fdata-sections
		-ffunction-sections
	)

	add_link_options(
		-flto
    	-Wl,--gc-sections,-s
	)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	message(STATUS "Configuring Release build: stripping debug info")
	add_compile_options(
		-O3
		-flto
		-fdata-sections
		-ffunction-sections
	)

	add_link_options(
		-flto
    	-Wl,--gc-sections,-s
	)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "Configuring Debug build: fast compile")
	add_compile_options(
		-DDEBUG
		-O0
		-g3
		-fno-omit-frame-pointer
	)

	add_link_options(
	)
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/packages")

set(CMAKE_BUILD_RPATH "$ORIGIN/../lib")
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/lib)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/lib)
endif()


# Packages
find_package(sdl CONFIG REQUIRED)
find_package(images CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_package(slang CONFIG REQUIRED)
find_package(compiler CONFIG REQUIRED)

# Add subdirectories
add_subdirectory(src/engine)

add_custom_target(delete_cache ALL
	COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/cache"
	COMMENT "---- Deleting folder recursively ----"
)

#compile modules
file(GLOB MODULE_DIRS LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/*"
)

include(${CMAKE_SOURCE_DIR}/cmake/helper.cmake)

#ToDo add a way to only depend on specific versions of programs and have a way to deny copies of the same program

file(GLOB CONFIG_FILES "${CMAKE_SOURCE_DIR}/src/modules/*/config.txt")

get_module_order(MODULE_ORDER
    MODULE_FILES ${CONFIG_FILES}
)

foreach(MODULE IN LISTS MODULE_ORDER)
	get_filename_component(MODULE_NAME ${MODULE} NAME)
	message(STATUS "module: ${MODULE_NAME}")
	set(MODULE_OUT_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/modules/${MODULE_NAME}")
	set(MOD_PATH "modules/${MODULE_NAME}")
	set(GENERATE_EXE OFF)
	set(MAKE_SO OFF)
	set(AUTO_COMP ON)
	set(AUTO_COPY ON)
	set(COMPILE_SHADERS ON)

	add_custom_target(conf_copy_${MODULE_NAME} ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${MODULE}/config.txt
			${MODULE_OUT_DIR}/config.txt
		COMMENT "---- Copying config ----"
	)

	if (EXISTS "${MODULE}/CMakeLists.txt")
	add_subdirectory(${MODULE})
	endif()

	if(${AUTO_COMP})
	compile_module(MAKE_SO ${MAKE_SO})
	endif()

	if(${AUTO_COPY})
	copy_assets()
	endif()

	if(${COMPILE_SHADERS})
	compile_shaders()
	endif()

	if (${GENERATE_EXE})
	compile_executable(${MODULE_NAME})
	endif()
endforeach()