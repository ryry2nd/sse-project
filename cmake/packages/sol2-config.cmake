add_subdirectory("${CMAKE_SOURCE_DIR}/libs/sol2" SOL_build)
add_library(sol2::sol2 ALIAS sol2)