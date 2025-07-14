set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# if your a crackhead and want to do something stupid like change the supported api's then go ahead, but remember it may or may not work
# keep in mind if you change these after you already build it, you gotta delete your build folder and spend 30 minutes compiling again. btw good luck

set(USE_VULKAN OFF CACHE BOOL "Enable Vulkan support")
set(USE_OPENGL ON CACHE BOOL "Enable OpenGL support")
set(USE_GLES OFF CACHE BOOL "Enable OpenGL ES support")

# options are OpenGL, Vulkan, GLES, or Software (or technically anything really because its an else)
set(DEFAULT_API "OpenGL" CACHE STRING "Default graphics API")

# dll's are evil
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")

# disable security warnings
add_definitions(-D_CRT_SECURE_NO_WARNINGS)