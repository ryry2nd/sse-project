find_package(Python3 COMPONENTS Interpreter REQUIRED)

function(merge_compile_commands OUTPUT_FILE)
	if(NOT ARGN)
		message(FATAL_ERROR "merge_compile_commands: no input files provided")
	endif()

	set(CMD
		${Python3_EXECUTABLE}
		${CMAKE_SOURCE_DIR}/cmake/helper/merge.py
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