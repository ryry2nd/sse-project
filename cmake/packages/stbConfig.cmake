# “Do I look like I know what a JPEG is? I just want a picture of a god-dang hot dog!” - Hank Hill

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()

add_library(stb_truetype INTERFACE)
add_library(stb::stb ALIAS stb_truetype)

target_include_directories(stb_truetype INTERFACE
    "${stb_SOURCE_DIR}"
)