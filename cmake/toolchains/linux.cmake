set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER clang-20)
set(CMAKE_CXX_COMPILER clang++-20)

set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)


# if your a crackhead and want to do something stupid like change the supported api's then go ahead, but remember it may or may not work
# keep in mind if you change these after you already build it, you gotta delete your build folder and spend 30 minutes compiling again. btw good luck
set(SLANG_DOWNLOAD_PATH "https://github.com/shader-slang/slang/releases/download/v2026.8/slang-2026.8-linux-x86_64.zip" CACHE STRING "")
set(SLANGC_BIN "bin/slangc" CACHE STRING "")

set(PIP_PATH "bin/pip" CACHE STRING "")
set(PYTHON_PATH "bin/python" CACHE STRING "")

option(USE_VULKAN "Use Vulkan" OFF)
option(USE_OPENGL "Use OpenGl" ON)

set(PREFERRED_API "opengl" CACHE STRING "")