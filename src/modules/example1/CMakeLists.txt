file(GLOB CPP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB ALL_ITEMS CONFIGURE_DEPENDS
    "${CMAKE_BINARY_DIR}/include/*"
)

set(MODULE_DEPS
    glm::glm
	yaml-cpp::yaml-cpp
	SDL3::SDL3
	spdlog::spdlog
	Boost::multiprecision
	RenderingBase
	Scripting
	CustomMath
)

set(INCLUDE_FLAGS "")
foreach(PATH ${ALL_ITEMS})
    if(IS_DIRECTORY ${PATH})
        list(APPEND INCLUDE_FLAGS "-I${PATH}")
    endif()
endforeach()

set(MOD_PATH "modules/example1")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(MODULE_FILE ${MODULE_OUT_DIR}/module.bc)

    add_custom_command(
        OUTPUT ${MODULE_FILE}
        COMMAND clang++-20
			-std=c++20
            -emit-llvm -c -O2 -flto -fdata-sections -ffunction-sections -fno-omit-frame-pointer
            ${INCLUDE_FLAGS}
            ${CPP_SOURCES}
			"-DMODULE_PATH=\\\"${MOD_PATH}\\\""
            -o ${MODULE_FILE}
        DEPENDS ${CPP_SOURCES} ${MODULE_DEPS}
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(MODULE_FILE ${MODULE_OUT_DIR}/module.ll)

    add_custom_command(
        OUTPUT ${MODULE_FILE}
        COMMAND clang++-20
			-std=c++20
            -emit-llvm -S -DDEBUG -O1 -g3 -fno-omit-frame-pointer
            ${INCLUDE_FLAGS}
            ${CPP_SOURCES}
			"-DMODULE_PATH=\\\"${MOD_PATH}\\\""
            -o ${MODULE_FILE}
        DEPENDS ${CPP_SOURCES} ${MODULE_DEPS}
    )
endif()

add_custom_target(MyModule ALL
    DEPENDS ${MODULE_FILE}
)