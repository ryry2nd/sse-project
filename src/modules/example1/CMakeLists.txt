file(GLOB CPP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

set(ALL_ITEMS
    "-I${CMAKE_BINARY_DIR}/include/Engine"
    "-I${CMAKE_BINARY_DIR}/include/extern"
)

set(MODULE_DEPS
    glm::glm
    yaml-cpp::yaml-cpp
    SDL3::SDL3
    spdlog::spdlog
    RenderingBase
    Scripting
)

set(MOD_PATH "modules/example1")

# ---------------------------------------------------
# MODULE BUILD OUTPUTS
# ---------------------------------------------------

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(MODULE_FILE ${MODULE_OUT_DIR}/module.bc)

    add_custom_command(
        OUTPUT ${MODULE_FILE}
        COMMAND clang++-20
            -std=c++20
            -emit-llvm -c -O2 -flto -fdata-sections -ffunction-sections -fno-omit-frame-pointer
            ${ALL_ITEMS}
            ${CPP_SOURCES}
            "-DMODULE_PATH=\\\"${MOD_PATH}\\\""
            -o ${MODULE_FILE}

        DEPENDS
            ${CPP_SOURCES}
            ${MODULE_DEPS}
            copy_src_headers   # <-- IMPORTANT: only reference existing target
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(MODULE_FILE ${MODULE_OUT_DIR}/module.ll)

    add_custom_command(
        OUTPUT ${MODULE_FILE}
        COMMAND clang++-20
            -std=c++20
            -emit-llvm -S -DDEBUG -O0 -g3 -fno-omit-frame-pointer
            ${ALL_ITEMS}
            ${CPP_SOURCES}
            "-DMODULE_PATH=\\\"${MOD_PATH}\\\""
            -o ${MODULE_FILE}

        DEPENDS
            ${CPP_SOURCES}
            ${MODULE_DEPS}
            copy_src_headers   # <-- same here
    )
endif()

# ---------------------------------------------------
# FINAL TARGET
# ---------------------------------------------------

add_custom_target(MyModule_build DEPENDS ${MODULE_FILE})

# this ensures engine-level header prep runs first
add_dependencies(MyModule_build copy_src_headers)

add_custom_target(MyModule ALL
    DEPENDS MyModule_build
)