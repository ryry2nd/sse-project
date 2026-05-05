include(FetchContent)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v2.0.8
)
FetchContent_MakeAvailable(glad)

set(GLAD_OUT ${glad_BINARY_DIR}/GLAD_BUILD)
set(VENV_DIR ${glad_BINARY_DIR}/.buildenv)

set(OPENGL_PATH ${GLAD_OUT}/OpenGL)
set(VULKAN_PATH ${GLAD_OUT}/vulkan)

file(MAKE_DIRECTORY ${GLAD_OUT})
file(MAKE_DIRECTORY ${VENV_DIR})

set(PIP_PATH "" CACHE STRING "the path to run pip")
set(PYTHON_PATH "" CACHE STRING "the path to run pip")

set(GLAD_COMMAND "
    ${Python3_EXECUTABLE} -m venv ${VENV_DIR} && \
    ${VENV_DIR}/${PYTHON_PATH} -m pip install --upgrade pip && \
    ${VENV_DIR}/${PIP_PATH} install -r ${glad_SOURCE_DIR}/requirements.txt")

if(USE_OPENGL AND NOT EXISTS ${OPENGL_PATH})
  string(APPEND GLAD_COMMAND "&& ${VENV_DIR}/${PYTHON_PATH} -m glad --api gl:core=4.6 --out-path ${OPENGL_PATH}")
endif()

if(USE_VULKAN AND NOT EXISTS ${VULKAN_PATH})
  string(APPEND GLAD_COMMAND "&& ${VENV_DIR}/${PYTHON_PATH} -m glad --api vulkan=1.4 --out-path ${VULKAN_PATH}")
endif()

if (NOT EXISTS ${VULKAN_PATH} AND NOT EXISTS ${OPENGL_PATH})
  execute_process(
    COMMAND bash -c "${GLAD_COMMAND}"
    WORKING_DIRECTORY ${glad_SOURCE_DIR}
  )
endif()

if(USE_OPENGL)
  file(GLOB gl_sources "${GLAD_OUT}/OpenGL/src/*.c")
  add_library(gl_library STATIC ${gl_sources})
  target_compile_options(gl_library INTERFACE -w)
  target_include_directories(gl_library PUBLIC ${GLAD_OUT}/OpenGL/include)
  add_library(OPENGL::OPENGL ALIAS gl_library)
endif()

if(USE_VULKAN)
  file(GLOB vulkan_sources "${GLAD_OUT}/vulkan/src/*.c")
  add_library(vulkan_library STATIC ${vulkan_sources})
  target_compile_options(vulkan_library INTERFACE -w)
  target_include_directories(vulkan_library PUBLIC ${GLAD_OUT}/vulkan/include)
  add_library(VULKAN::VULKAN ALIAS vulkan_library)
endif()