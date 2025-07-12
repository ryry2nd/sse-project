add_subdirectory("${CMAKE_SOURCE_DIR}/libs/yaml-cpp" YAML_build)
add_library(yaml-cpp::yaml-cpp ALIAS yaml-cpp)