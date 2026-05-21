file(GLOB CPP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

set(ALL_ITEMS
    "-I${CMAKE_BINARY_DIR}/include/Engine"
    "-I${glm_SOURCE_DIR}"
	"-I${sdl_SOURCE_DIR}/include"
)

set(MOD_PATH "modules/example1")

option(MAKE_SO "" ON)

if (SSE_MINIMAL OR MAKE_SO)
	set(MODULE_FILE ${MODULE_OUT_DIR}/module.so)

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(MODULE_FLAGS -shared -O2 -fPIC -flto -fdata-sections -ffunction-sections -fno-omit-frame-pointer -Wl,--gc-sections)
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(MODULE_FLAGS -shared -fPIC -DDEBUG -O0 -g3 -fno-omit-frame-pointer)
	endif()
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(MODULE_FILE ${MODULE_OUT_DIR}/module.bc)
		set(MODULE_FLAGS -emit-llvm -c -O2 -flto -fdata-sections -ffunction-sections -fno-omit-frame-pointer)
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(MODULE_FILE ${MODULE_OUT_DIR}/module.ll)
		set(MODULE_FLAGS -emit-llvm -S -DDEBUG -O0 -g3 -fno-omit-frame-pointer)
	endif()
endif()

add_custom_command(
	OUTPUT ${MODULE_FILE}
	COMMAND ${CMAKE_CXX_COMPILER_LAUNCHER} ${CMAKE_CXX_COMPILER}
		-std=c++20
		${MODULE_FLAGS}
		${ALL_ITEMS}
		${CPP_SOURCES}
		"-DMODULE_PATH=\\\"${MOD_PATH}\\\""
		-o ${MODULE_FILE}

	DEPENDS
		${CPP_SOURCES}
		copy_src_headers
)

# ---------------------------------------------------
# FINAL TARGET
# ---------------------------------------------------

add_custom_target(MyModule_build DEPENDS ${MODULE_FILE})

add_dependencies(MyModule_build copy_src_headers)

add_custom_target(MyModule ALL
    DEPENDS MyModule_build
)

add_custom_target(copy_assets ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"${MODULE_OUT_DIR}/assets"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${MODULE}/assets
		${MODULE_OUT_DIR}/assets
	COMMENT "---- Copying assets ----"
)

if (SSE_MINIMAL)
add_custom_target(compile_slang_shaders ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"${MODULE_OUT_DIR}/shaders"
    COMMAND ${slang_SOURCE_DIR}/${SLANGC_BIN}
        "${CMAKE_SOURCE_DIR}/src/modules/example1/shaders/floatCube.slang"
        -O3 -line-directive-mode none -matrix-layout-column-major
        -target spirv -stage vertex -entry vertMain
        -o "${MODULE_OUT_DIR}/shaders/floatCube.vert.spv"

    COMMAND ${slang_SOURCE_DIR}/${SLANGC_BIN}
        "${CMAKE_SOURCE_DIR}/src/modules/example1/shaders/floatCube.slang"
        -O3 -line-directive-mode none -matrix-layout-column-major
        -target spirv -stage fragment -entry fragMain
        -o "${MODULE_OUT_DIR}/shaders/floatCube.frag.spv"
	COMMENT "---- Compiling Shaders ----"
)
add_dependencies(compile_slang_shaders copy_assets)
else()
add_custom_target(copy_shaders ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"${MODULE_OUT_DIR}/shaders"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${MODULE}/shaders
		${MODULE_OUT_DIR}/shaders
	COMMENT "---- Copying Shaders ----"
)
endif()