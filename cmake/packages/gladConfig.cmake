find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad)
set(GLAD_OUT ${CMAKE_CURRENT_BINARY_DIR}/GLAD_BUILD)
set(VENV_DIR ${CMAKE_CURRENT_BINARY_DIR}/.buildenv)

file(MAKE_DIRECTORY ${GLAD_OUT})
file(MAKE_DIRECTORY ${VENV_DIR})


set(GLAD_COMMAND "
    ${Python3_EXECUTABLE} -m venv ${VENV_DIR} && \
    ${VENV_DIR}/bin/pip install --upgrade pip && \
    ${VENV_DIR}/bin/pip install -r requirements.txt")

if(USE_OPENGL)
  string(APPEND GLAD_COMMAND "&& ${VENV_DIR}/bin/python -m glad --api gl:core=4.6 --out-path ${GLAD_OUT}/OpenGL")
endif()

if(USE_VULKAN)
  string(APPEND GLAD_COMMAND "&& ${VENV_DIR}/bin/python -m glad --api vulkan=1.4 --out-path ${GLAD_OUT}/vulkan")
endif()

execute_process(
  COMMAND bash -c "${GLAD_COMMAND}"
  WORKING_DIRECTORY ${GLAD_DIR}
)

if(USE_OPENGL)
  file(GLOB gl_sources "${GLAD_OUT}/OpenGL/src/*.c")
  add_library(gl_library STATIC ${gl_sources})
  target_include_directories(gl_library PUBLIC ${GLAD_OUT}/OpenGL/include)
  add_library(OPENGL::OPENGL ALIAS gl_library)
endif()

if(USE_VULKAN)
  file(GLOB vulkan_sources "${GLAD_OUT}/vulkan/src/*.c")
  add_library(vulkan_library STATIC ${vulkan_sources})
  target_include_directories(vulkan_library PUBLIC ${GLAD_OUT}/vulkan/include)
  add_library(VULKAN::VULKAN ALIAS vulkan_library)
endif()