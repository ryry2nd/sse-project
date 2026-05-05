include(FetchContent)

set(SLANG_DOWNLOAD_PATH "" CACHE STRING "Download path for slang")
set(SLANGC_BIN "" CACHE STRING "the file cmake needs to copy for slangc")
set(SLANG_COMPILER_LIB "" CACHE STRING "the lib cmake needs to copy for the compiler")
set(SLANG_GLSLANG_LIB "" CACHE STRING "the lib cmake needs to copy for glslang")
set(SLANGC_BIN_OUT "" CACHE STRING "the file cmake needs to copy for slangc")
set(SLANG_COMPILER_LIB_OUT "" CACHE STRING "the lib cmake needs to copy for the compiler")
set(SLANG_GLSLANG_LIB_OUT "" CACHE STRING "the lib cmake needs to copy for glslang")

FetchContent_Declare(
  slang
  URL ${SLANG_DOWNLOAD_PATH}
)

FetchContent_MakeAvailable(slang)


add_custom_target(copy_slang ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${SLANGC_BIN_OUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${SLANG_COMPILER_LIB_OUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${SLANG_GLSLANG_LIB_OUT}

    # copy slangc
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${slang_SOURCE_DIR}/${SLANGC_BIN}
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${SLANGC_BIN_OUT}

    # copy only slang compiler libs
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${slang_SOURCE_DIR}/${SLANG_COMPILER_LIB}
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${SLANG_COMPILER_LIB_OUT}

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${slang_SOURCE_DIR}/${SLANG_GLSLANG_LIB}
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${SLANG_GLSLANG_LIB_OUT}
)