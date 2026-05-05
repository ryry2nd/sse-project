include(FetchContent)

# set(slangBuild "${CMAKE_BINARY_DIR}/slang-build")
set(OUT_BIN ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin-tools)
set(OUT_LIB ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/lib)

FetchContent_Declare(
  slang
  URL https://github.com/shader-slang/slang/releases/download/v2026.8/slang-2026.8-linux-x86_64.zip
)

FetchContent_MakeAvailable(slang)


add_custom_target(copy_slang ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_BIN}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_LIB}

    # copy slangc
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${slang_SOURCE_DIR}/bin/slangc
        ${OUT_BIN}/

    # copy only slang compiler libs
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${slang_SOURCE_DIR}/lib/libslang-compiler.so.0.2026.8
        ${OUT_LIB}/

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${slang_SOURCE_DIR}/lib/libslang-glslang-2026.8.so
        ${OUT_LIB}/
)