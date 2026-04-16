include(ExternalProject)

set(slangBuild "${CMAKE_BINARY_DIR}/slang-build")
set(OUT_BIN ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin)
set(OUT_LIB ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/lib)

ExternalProject_Add(slang
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/slang
    BINARY_DIR ${slangBuild}

    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DSLANG_ENABLE_SLANGC=ON
        -DSLANG_ENABLE_TESTS=OFF
        -DSLANG_ENABLE_EXAMPLES=OFF
        -DSLANG_ENABLE_GFX=OFF
        -DSLANG_ENABLE_SLANG_GLSLANG=OFF
        -DSLANG_SLANG_LLVM_FLAVOR=DISABLE
        -DSLANG_EMBED_CORE_MODULE=ON
        -DSLANG_EMBED_CORE_MODULE_SOURCE=ON
        -DSLANG_ENABLE_CUDA=OFF
        -DSLANG_ENABLE_OPTIX=OFF
        -DSLANG_ENABLE_NVAPI=OFF
        -DSLANG_ENABLE_SLANGD=OFF
        -DSLANG_ENABLE_SLANGRT=OFF
        -DSLANG_ENABLE_REPLAYER=OFF
        -DSLANG_USE_SYSTEM_MINIZ=OFF
        -DSLANG_USE_SYSTEM_LZ4=OFF
        -DSLANG_USE_SYSTEM_GLSLANG=OFF
        -DSLANG_ENABLE_SLANG_RHI=OFF
        -Wunused-variable
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

    INSTALL_COMMAND ""
)

add_custom_target(copy_slang ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_BIN}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_LIB}

    # copy slangc
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${slangBuild}/${CMAKE_BUILD_TYPE}/bin/slangc
        ${OUT_BIN}/

    # copy only slang compiler libs
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${slangBuild}/${CMAKE_BUILD_TYPE}/lib/libslang-compiler*
        ${OUT_LIB}/
)
add_dependencies(copy_slang slang)