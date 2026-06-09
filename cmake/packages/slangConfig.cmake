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