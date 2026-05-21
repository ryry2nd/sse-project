include(FetchContent)

set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)
set(BUILD_TESTING OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_CPP_BUILD_CONTRIB OFF)
set(YAML_CPP_BUILD_SHARED OFF)
set(YAML_CPP_BUILD_STATIC ON)
set(YAML_CPP_BUILD_DOCS OFF)
set(YAML_CPP_INSTALL OFF)


FetchContent_Declare(
	yaml
	GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
	GIT_TAG yaml-cpp-0.9.0
)
FetchContent_MakeAvailable(yaml)