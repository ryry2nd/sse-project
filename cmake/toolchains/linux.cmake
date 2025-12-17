set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# if your a crackhead and want to do something stupid like change the supported api's then go ahead, but remember it may or may not work
# keep in mind if you change these after you already build it, you gotta delete your build folder and spend 30 minutes compiling again. btw good luck

set(USE_VULKAN OFF CACHE BOOL "Enable Vulkan support")
set(USE_OPENGL ON CACHE BOOL "Enable OpenGL support")

# options are OpenGL, Vulkan, GLES, or Software (or technically anything really because its an else)
set(DEFAULT_API "OpenGL" CACHE STRING "Default graphics API")