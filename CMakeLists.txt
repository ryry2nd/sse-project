cmake_minimum_required(VERSION 3.15)

# SSE is a Small Engine
project(sse-project VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(USE_VULKAN "Enable Vulkan support" OFF)
option(USE_OPENGL "Enable OpenGL support" OFF)
option(USECMDCLANG "Use the commandline clang" OFF)

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
		-Wl,--gc-sections
)
else()
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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/bin)

# Packages
find_package(sdl REQUIRED)
find_package(sdl_image REQUIRED)
find_package(sdl_ttf REQUIRED)
find_package(glm REQUIRED)
find_package(boost_multiprecision REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(glad REQUIRED)
find_package(spdlog REQUIRED)

find_package(slang REQUIRED)

if (USECMDCLANG STREQUAL "OFF")
        find_package(clang REQUIRED)
endif()

# Add subdirectories
add_subdirectory(src/engine)
add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE
        SDL3::SDL3
        CustomMath
        RenderingBase
        # BigObjects
        Scripting
        spdlog::spdlog
)

add_custom_target(delete_cache ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/cache"
        COMMENT "---- Deleting folder recursively ----"
)

# copy headers
file(GLOB_RECURSE MATH_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/engine/CustomMath/*.hpp
)
file(GLOB_RECURSE BIG_OBJECT_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/engine/BigObjects/*.hpp
)
file(GLOB_RECURSE SCRIPTING_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/engine/Scripting/*.hpp
)
file(GLOB_RECURSE RENDERING_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/engine/Rendering/*.hpp
)
list(FILTER RENDERING_HEADERS INCLUDE REGEX ".*/Rendering/[^/]+$")

add_custom_target(copy_headers ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine/CustomMath
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine/BigObjects
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine/Rendering
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine/Scripting
        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${MATH_HEADERS}
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine/CustomMath
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${BIG_OBJECT_HEADERS}
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine/BigObjects
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${RENDERING_HEADERS}
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine/Rendering
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SCRIPTING_HEADERS}
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/engine/Scripting
        COMMENT "---- Copying headers ----"
)

# Post-build asset copy
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/src/modules
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/modules
        COMMENT "---- Copying assets ----"
)

add_custom_target(copy_compile_command ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
                ${CMAKE_CURRENT_BINARY_DIR}/..
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "---- Copying compile commands ----"
)