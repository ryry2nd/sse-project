file(GLOB_RECURSE engine_SOURCES "*.cpp")

add_library(engine SHARED ${engine_SOURCES})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})