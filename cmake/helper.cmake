find_package(Python3 COMPONENTS Interpreter REQUIRED)

function(merge_compile_commands OUTPUT_FILE)
	if(NOT ARGN)
		message(FATAL_ERROR "merge_compile_commands: no input files provided")
	endif()

	set(CMD
		${Python3_EXECUTABLE}
		${CMAKE_SOURCE_DIR}/cmake/merge.py
		${OUTPUT_FILE}
	)

	foreach(FILE ${ARGN})
		if(NOT EXISTS ${FILE})
			message(WARNING "Skipping missing compile DB: ${FILE}")
			return()
		endif()

		list(APPEND CMD ${FILE})
	endforeach()

	execute_process(
		COMMAND ${CMD}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE RES
		OUTPUT_VARIABLE OUT
		ERROR_VARIABLE ERR
	)

	if(NOT RES EQUAL 0)
		message(FATAL_ERROR
			"Failed to merge compile_commands.json files\n"
			"Output:\n${OUT}\n"
			"Error:\n${ERR}"
		)
	endif()

	message(STATUS "${OUT}")
	message(STATUS "Merged compile_commands → ${OUTPUT_FILE}")
endfunction()

function(compile_executable EXE_NAME)
	add_executable(${EXE_NAME} ${CMAKE_SOURCE_DIR}/src/main.cpp)

	target_link_libraries(${EXE_NAME} PRIVATE
		RenderingBase
		Scripting
		ScriptingJIT
	)
	target_compile_definitions(${EXE_NAME} PRIVATE DEFAULT_MODULE=\"${MOD_PATH}\")
endfunction()


function(compile_module MAKE_SO)
	file(GLOB CPP_SOURCES CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	)

	set(ALL_ITEMS
		"-I${CMAKE_BINARY_DIR}/include/Engine"
		"-I${CMAKE_BINARY_DIR}/include/Rendering"
		"-I${glm_SOURCE_DIR}"
		"-I${sdl_SOURCE_DIR}/include"
	)

	if (SSE_MINIMAL OR MAKE_SO)
		if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(MODULE_FILE ${MODULE_OUT_DIR}/module.dll)
		else()
		set(MODULE_FILE ${MODULE_OUT_DIR}/module.so)
		endif()

		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			set(MODULE_FLAGS -shared -O2 -fPIC -fdata-sections -ffunction-sections -fno-omit-frame-pointer -Wl,--gc-sections)
		elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(MODULE_FLAGS -shared -fPIC -DDEBUG -O0 -g3 -fno-omit-frame-pointer)
		endif()
	else()
		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			set(MODULE_FILE ${MODULE_OUT_DIR}/module.bc)
			set(MODULE_FLAGS -emit-llvm -c -O2 -fdata-sections -ffunction-sections -fno-omit-frame-pointer)
		elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(MODULE_FILE ${MODULE_OUT_DIR}/module.ll)
			set(MODULE_FLAGS -emit-llvm -S -DDEBUG -O0 -g3 -fno-omit-frame-pointer)
		endif()
	endif()

	if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(TARGET -target ${CMAKE_CXX_COMPILER_TARGET})
	endif()

	add_custom_command(
		OUTPUT ${MODULE_FILE}
		COMMAND ${CMAKE_CXX_COMPILER_LAUNCHER} ${CMAKE_CXX_COMPILER}
			-std=c++20
			${MODULE_FLAGS}
			${ALL_ITEMS}
			${CPP_SOURCES}
			${TARGET}
			"-DMODULE_PATH=\\\"${MOD_PATH}\\\""
			-o ${MODULE_FILE}

		DEPENDS
			${CPP_SOURCES}
			copy_src_headers
	)

	add_custom_target(MyModule_build DEPENDS ${MODULE_FILE})

	add_dependencies(MyModule_build copy_src_headers)

	add_custom_target(MyModule ALL
		DEPENDS MyModule_build
	)

	string(JOIN " " MODULE_FLAGS_STR ${MODULE_FLAGS})
	string(JOIN " " ALL_ITEMS_STR ${ALL_ITEMS})

	string(JOIN " " CC_ENTRY
	"{"
	"\"directory\":\"${CMAKE_SOURCE_DIR}\","
	"\"file\":\"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp\","
	"\"command\":\"${CMAKE_CXX_COMPILER} -std=c++20 ${MODULE_FLAGS_STR} ${ALL_ITEMS_STR} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp -target ${CMAKE_CXX_COMPILER_TARGET} -DMODULE_PATH=\\\"${MOD_PATH}\\\" -o ${MODULE_FILE}\""
	"}"
	)

	file(WRITE
		${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
		"[${CC_ENTRY}]"
	)
	merge_compile_commands(
		${CMAKE_BINARY_DIR}/../compile_commands.json
		${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
		${CMAKE_BINARY_DIR}/compile_commands.json
	)
endfunction()

function(copy_assets)
	add_custom_target(copy_assets_target ALL
		COMMAND ${CMAKE_COMMAND} -E make_directory
			"${MODULE_OUT_DIR}/assets"
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${MODULE}/assets
			${MODULE_OUT_DIR}/assets
		COMMENT "---- Copying assets ----"
	)

	if (SSE_MINIMAL)

	if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(SLANGC_CMD "wine" "${slang_SOURCE_DIR}/${SLANGC_BIN}")
	else()
		set(SLANGC_CMD "${slang_SOURCE_DIR}/${SLANGC_BIN}")
	endif()

	file(GLOB SLANG_SHADERS
		"${CMAKE_SOURCE_DIR}/src/modules/example1/shaders/*.slang"
	)

	list(FILTER SLANG_SHADERS EXCLUDE REGEX "/\\.[^/]*$")

	add_custom_target(compile_slang_shaders ALL
		COMMAND ${CMAKE_COMMAND} -E make_directory
			"${MODULE_OUT_DIR}/shaders"
	)

	foreach(SHADER ${SLANG_SHADERS})

		get_filename_component(NAME ${SHADER} NAME_WE)

		add_custom_command(
			TARGET compile_slang_shaders
			COMMAND ${SLANGC_CMD}
				"${SHADER}"
				-O3 -line-directive-mode none -matrix-layout-column-major
				-target spirv -stage vertex
				-DGL
				-entry vertMain
				-o "${MODULE_OUT_DIR}/shaders/${NAME}.vert.spv"

			COMMAND ${SLANGC_CMD}
				"${SHADER}"
				-O3 -line-directive-mode none -matrix-layout-column-major
				-target spirv -stage fragment
				-DGL
				-entry fragMain
				-o "${MODULE_OUT_DIR}/shaders/${NAME}.frag.spv"

			COMMENT "Compiling ${NAME}"
		)

	endforeach()

	add_dependencies(compile_slang_shaders copy_assets_target)
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
endfunction()