include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(spdlog)

add_custom_target(copy_spdlog_headers ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${spdlog_SOURCE_DIR}/include/spdlog
				${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/spdlog
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${spdlog_SOURCE_DIR}/LICENSE
				${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/include/spdlog
		COMMENT "---- Copying spdlog headers ----"
)