add_library(sol2 INTERFACE)
add_library(sol2::sol2 ALIAS sol2)

file(GLOB LUA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/lua/*.c"
)
add_library(lua STATIC ${LUA_SOURCES})

target_include_directories(sol2 INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/lua"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/sol2/include"
)
target_link_libraries(sol2 INTERFACE lua)