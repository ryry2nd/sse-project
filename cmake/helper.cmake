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
	message(STATUS "Merged compile_commands -> ${OUTPUT_FILE}")
endfunction()

function(compile_executable EXE_NAME)
	add_executable(${MODULE_NAME}_exe ${CMAKE_SOURCE_DIR}/src/main.cpp)

	target_link_libraries(${MODULE_NAME}_exe PRIVATE
		Scripting
		RenderingBase
	)
	target_compile_definitions(${MODULE_NAME}_exe PRIVATE DEFAULT_MODULE=\"${MOD_PATH}\")
	set_target_properties(${MODULE_NAME}_exe PROPERTIES OUTPUT_NAME "${EXE_NAME}")
endfunction()


function(compile_module)
	set(oneValueArgs MAKE_SO)
    set(multiValueArgs DEPS)
    set(options)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	file(GLOB CPP_SOURCES CONFIGURE_DEPENDS
		"${MODULE}/src/*.cpp"
	)

	set(ALL_ITEMS
		"-I${CMAKE_BINARY_DIR}/include/Engine"
		"-I${CMAKE_BINARY_DIR}/include/Rendering"
		"-I${glm_SOURCE_DIR}"
		"-I${sdl_SOURCE_DIR}/include"
		"-I${MODULE}/include"
	)

	if (SSE_MINIMAL OR ARG_MAKE_SO)
		if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(MODULE_FILE ${MODULE_OUT_DIR}/module.dll)
		else()
		set(MODULE_FILE ${MODULE_OUT_DIR}/module.so)
		endif()

		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			set(MODULE_FLAGS -shared -Oz -fPIC -fdata-sections -ffunction-sections)
		elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(MODULE_FLAGS -shared -fPIC -DDEBUG -O0 -g3 -fno-omit-frame-pointer)
		endif()
	else()
		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			set(MODULE_FILE ${MODULE_OUT_DIR}/module.bc)
			set(MODULE_FLAGS -emit-llvm -c -O3 -fdata-sections -ffunction-sections)
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

	add_custom_target(${MODULE_NAME}_build DEPENDS ${MODULE_FILE})

	add_dependencies(${MODULE_NAME}_build copy_src_headers)

	add_custom_target(${MODULE_NAME}_target ALL
		DEPENDS ${MODULE_NAME}_build
	)

	string(JOIN " " MODULE_FLAGS_STR ${MODULE_FLAGS})
	string(JOIN " " ALL_ITEMS_STR ${ALL_ITEMS})

	foreach(FILE ${CPP_SOURCES})
		string(JOIN " " CC_ENTRY
		"{"
		"\"directory\":\"${CMAKE_SOURCE_DIR}\","
		"\"file\":\"${FILE}\","
		"\"command\":\"${CMAKE_CXX_COMPILER} -std=c++20 ${MODULE_FLAGS_STR} ${ALL_ITEMS_STR} ${FILE} -target ${CMAKE_CXX_COMPILER_TARGET} -DMODULE_PATH=\\\"${MOD_PATH}\\\" -o ${MODULE_FILE}\""
		"}"
		)
	endforeach()

	file(WRITE
		${MODULE_OUT_DIR}/compile_commands.json
		"[${CC_ENTRY}]"
	)
	merge_compile_commands(
		${CMAKE_BINARY_DIR}/../compile_commands.json
		${MODULE_OUT_DIR}/compile_commands.json
		${CMAKE_BINARY_DIR}/compile_commands.json
	)
endfunction()

function(copy_assets)
	if(EXISTS "${MODULE}/assets")
	add_custom_target(copy_assets_target_${MODULE_NAME} ALL
		COMMAND ${CMAKE_COMMAND} -E make_directory
			"${MODULE_OUT_DIR}/assets"
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${MODULE}/assets
			${MODULE_OUT_DIR}/assets
		COMMENT "---- Copying assets ----"
	)
	endif()

	if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(SLANGC_CMD "wine" "${slang_SOURCE_DIR}/${SLANGC_BIN}")
	else()
		set(SLANGC_CMD "${slang_SOURCE_DIR}/${SLANGC_BIN}")
	endif()

	if(EXISTS "${MODULE}/shaders")
	file(GLOB SLANG_SHADERS
		"${MODULE}/shaders/*.slang"
	)

	list(FILTER SLANG_SHADERS EXCLUDE REGEX "/\\.[^/]*$")

	add_custom_target(compile_slang_shaders_${MODULE_NAME} ALL
		COMMAND ${CMAKE_COMMAND} -E make_directory
			"${MODULE_OUT_DIR}/shaders/gl"
	)

	foreach(SHADER ${SLANG_SHADERS})

		get_filename_component(NAME ${SHADER} NAME_WE)

		add_custom_command(
			TARGET compile_slang_shaders_${MODULE_NAME}
			COMMAND ${SLANGC_CMD}
				"${SHADER}"
				-O3 -line-directive-mode none -matrix-layout-column-major
				-target spirv -stage vertex
				-DGL
				-entry vertMain
				-o "${MODULE_OUT_DIR}/shaders/gl/${NAME}.vert.spv"

			COMMAND ${SLANGC_CMD}
				"${SHADER}"
				-O3 -line-directive-mode none -matrix-layout-column-major
				-target spirv -stage fragment
				-DGL
				-entry fragMain
				-o "${MODULE_OUT_DIR}/shaders/gl/${NAME}.frag.spv"

			COMMENT "Compiling ${NAME}"
		)

	endforeach()
	endif()

	if(EXISTS "${MODULE}/assets" AND EXISTS "${MODULE}/shaders")
	add_dependencies(compile_slang_shaders_${MODULE_NAME} copy_assets_target_${MODULE_NAME})
	endif()
endfunction()

function(generate_conf)
	set(oneValueArgs ID SHORT_NAME VERSION)
    set(multiValueArgs DEPS)
    set(options)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	file(WRITE ${MODULE_OUT_DIR}/config.txt
        "ID=${ARG_ID}\n"
        "SHORT_NAME=${ARG_SHORT_NAME}\n"
        "VERSION=${ARG_VERSION}\n"
        "DEPS=${ARG_DEPS}\n"
    )
endfunction()