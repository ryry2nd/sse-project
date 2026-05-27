set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER clang-21)
set(CMAKE_C_COMPILER_TARGET x86_64-w64-windows-gnu)
set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER clang++-21)
set(CMAKE_CXX_COMPILER_TARGET x86_64-w64-windows-gnu)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

# disable security warnings
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# if your a crackhead and want to do something stupid like change the supported api's then go ahead, but remember it may or may not work
# keep in mind if you change these after you already build it, you gotta delete your build folder and spend 30 minutes compiling again. btw good luck

set(SLANG_DOWNLOAD_PATH "https://github.com/shader-slang/slang/releases/download/v2026.8/slang-2026.8-windows-x86_64.zip" CACHE STRING "")
set(SLANGC_BIN "bin/slangc.exe" CACHE STRING "")
set(SLANG_COMPILER_LIB "bin/slang-compiler.dll" CACHE STRING "")
set(SLANG_GLSLANG_LIB "bin/slang-glslang.dll" CACHE STRING "")
set(SLANGC_BIN_OUT "bin-tools" CACHE STRING "")
set(SLANG_COMPILER_LIB_OUT "bin-tools" CACHE STRING "")
set(SLANG_GLSLANG_LIB_OUT "bin-tools" CACHE STRING "")

set(PIP_PATH "Scripts/pip.exe" CACHE STRING "")
set(PYTHON_PATH "Scripts/python.exe" CACHE STRING "")

option(USE_VULKAN "Use Vulkan" OFF)
option(USE_OPENGL "Use OpenGl" ON)