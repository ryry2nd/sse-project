set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(TARGET_TRIPLE x86_64-w64-mingw32)
set(CMAKE_C_COMPILER   /usr/bin/clang-20)
set(CMAKE_CXX_COMPILER /usr/bin/clang++-20)
set(CMAKE_C_COMPILER_TARGET   ${TARGET_TRIPLE})
set(CMAKE_CXX_COMPILER_TARGET ${TARGET_TRIPLE})

set(CMAKE_LINKER /usr/bin/x86_64-w64-mingw32-ld)

set(MINGW_SYSROOT /usr/x86_64-w64-mingw32)
set(CMAKE_SYSROOT ${MINGW_SYSROOT})
set(CMAKE_FIND_ROOT_PATH ${MINGW_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

find_program(CMAKE_RC_COMPILER NAMES x86_64-w64-mingw32-windres)


# disable security warnings
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# if your a crackhead and want to do something stupid like change the supported api's then go ahead, but remember it may or may not work
# keep in mind if you change these after you already build it, you gotta delete your build folder and spend 30 minutes compiling again. btw good luck

set(SLANG_DOWNLOAD_PATH "https://github.com/shader-slang/slang/releases/download/v2026.8/slang-2026.8-windows-x86_64.zip" CACHE STRING "")
set(SLANGC_BIN "bin/slangc.exe" CACHE STRING "")

set(PIP_PATH "bin/pip" CACHE STRING "")
set(PYTHON_PATH "bin/python" CACHE STRING "")

option(USE_VULKAN "Use Vulkan" OFF)
option(USE_OPENGL "Use OpenGl" ON)