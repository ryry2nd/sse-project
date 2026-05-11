include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(spdlog)

# add_custom_target(copy_spdlog_headers ALL
# 		COMMAND ${CMAKE_COMMAND} -E copy_directory
# 				${spdlog_SOURCE_DIR}/include/spdlog
# 				${CMAKE_CURRENT_BINARY_DIR}/include/extern/spdlog
# 		COMMENT "---- Copying spdlog headers ----"
# )