find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad)
set(GLAD_OUT ${CMAKE_CURRENT_BINARY_DIR}/GLAD_BUILD)
set(VENV_DIR ${CMAKE_CURRENT_BINARY_DIR}/.buildenv)

file(MAKE_DIRECTORY ${GLAD_OUT})
file(MAKE_DIRECTORY ${VENV_DIR})


set(GLAD_API "")

if(USE_OPENGL)
  string(APPEND GLAD_API "--api gl:core=4.6 ")
endif()

if(USE_GLES)
  string(APPEND GLAD_API "--api gles2=3.2 ")
endif()

if(USE_VULKAN)
  string(APPEND GLAD_API "--api vulkan=1.4 ")
endif()

execute_process(
    COMMAND bash -c "
    ${Python3_EXECUTABLE} -m venv ${VENV_DIR} && \
    ${VENV_DIR}/bin/pip install --upgrade pip && \
    ${VENV_DIR}/bin/pip install -r requirements.txt && \
    ${VENV_DIR}/bin/python -m glad ${GLAD_API} --out-path ${GLAD_OUT}
            "
    WORKING_DIRECTORY ${GLAD_DIR}
)

file(GLOB GLAD_SOURCES "${GLAD_OUT}/src/*.c")
add_library(glad_static STATIC ${GLAD_SOURCES})
target_include_directories(glad_static PUBLIC ${GLAD_OUT}/include)
add_library(glad INTERFACE)
add_library(glad::glad ALIAS glad)
target_link_libraries(glad INTERFACE glad_static)

if (USE_VULKAN)
  add_library(vulk_headers INTERFACE)
  add_library(vulk_headers::vulk_headers ALIAS vulk_headers)
  target_include_directories(vulk_headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/libs/vulkan-headers/include")
  target_link_libraries(glad INTERFACE vulk_headers)
endif()