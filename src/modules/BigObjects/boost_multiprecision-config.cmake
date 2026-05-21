include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	multiprecision
	GIT_REPOSITORY https://github.com/boostorg/multiprecision.git
	GIT_TAG boost-1.91.0
)
FetchContent_MakeAvailable(multiprecision)

FetchContent_Declare(
	boost-config
	GIT_REPOSITORY https://github.com/boostorg/config.git
	GIT_TAG boost-1.91.0
)
FetchContent_MakeAvailable(boost-config)