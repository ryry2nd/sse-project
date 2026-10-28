include(FetchContent)

FetchContent_Declare(
    Tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG v0.14.1
    GIT_SHALLOW TRUE
)

set(TRACY_ENABLE ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(Tracy)

set_target_properties(TracyClient PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)