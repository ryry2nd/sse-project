set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER clang-20)
set(CMAKE_CXX_COMPILER clang++-20)
set(CMAKE_C_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_CXX_COMPILER_TARGET x86_64-w64-mingw32)

set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

set(CMAKE_EXE_LINKER_FLAGS_INIT    "-fuse-ld=lld -L/usr/lib/gcc/x86_64-w64-mingw32/13-win32 -L/usr/x86_64-w64-mingw32/lib")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld -L/usr/lib/gcc/x86_64-w64-mingw32/13-win32 -L/usr/x86_64-w64-mingw32/lib")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=lld -L/usr/lib/gcc/x86_64-w64-mingw32/13-win32 -L/usr/x86_64-w64-mingw32/lib")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_C_FLAGS_INIT   "-target x86_64-w64-mingw32 -D_WIN32 --sysroot=/usr/x86_64-w64-mingw32")
set(CMAKE_CXX_FLAGS_INIT "-target x86_64-w64-mingw32 -D_WIN32 --sysroot=/usr/x86_64-w64-mingw32 -isystem /usr/lib/gcc/x86_64-w64-mingw32/13-win32/include/c++ -isystem /usr/lib/gcc/x86_64-w64-mingw32/13-win32/include/c++/x86_64-w64-mingw32 -isystem /usr/lib/gcc/x86_64-w64-mingw32/13-win32/include/c++/backward")

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

set(PIP_PATH "bin/pip" CACHE STRING "")
set(PYTHON_PATH "bin/python" CACHE STRING "")

option(USE_VULKAN "Use Vulkan" OFF)
option(USE_OPENGL "Use OpenGl" ON)