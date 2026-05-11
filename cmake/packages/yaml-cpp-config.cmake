include(FetchContent)

set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)
set(BUILD_TESTING OFF)
option(YAML_CPP_BUILD_TOOLS OFF)
option(YAML_CPP_BUILD_TESTS OFF)
option(YAML_CPP_INSTALL OFF)


FetchContent_Declare(
	yaml
	GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
	GIT_TAG yaml-cpp-0.9.0
)
FetchContent_MakeAvailable(yaml)