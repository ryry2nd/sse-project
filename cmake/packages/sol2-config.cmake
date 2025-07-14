add_library(sol2 INTERFACE)
add_library(sol2::sol2 ALIAS sol2)

file(GLOB LUA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/lua/*.c"
)

list(REMOVE_ITEM LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/libs/lua/onelua.c")

set(BUILD_SHARED_LIBS ON)
add_library(lua SHARED ${LUA_SOURCES})
target_include_directories(lua PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/lua")

target_include_directories(sol2 INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/sol2/include"
)
target_link_libraries(sol2 INTERFACE lua)