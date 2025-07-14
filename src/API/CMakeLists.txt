if(USE_OPENGL)
    file(GLOB_RECURSE GL_SOURCES "opengl/*.cpp")
endif()


set(API_SOURCES "")
list(APPEND API_SOURCES ${GL_SOURCES})# ${VK_SOURCES} ${GLES_SOURCES})

add_library(API SHARED ${API_SOURCES})

if(USE_OPENGL)
    target_include_directories(API PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/opengl)
endif()