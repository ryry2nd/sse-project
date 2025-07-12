find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad)
set(GLAD_OUT ${CMAKE_CURRENT_BINARY_DIR}/GLAD_BUILD)
set(VENV_DIR ${CMAKE_CURRENT_BINARY_DIR}/.buildenv)

file(MAKE_DIRECTORY ${GLAD_OUT})
file(MAKE_DIRECTORY ${VENV_DIR})

execute_process(
    COMMAND bash -c "
    ${Python3_EXECUTABLE} -m venv ${VENV_DIR} && \
    ${VENV_DIR}/bin/pip install --upgrade pip && \
    ${VENV_DIR}/bin/pip install --upgrade glad2 && \
    ${VENV_DIR}/bin/python -m glad --out-path ${GLAD_OUT} --api=gl:core=4.6
            "
    )

file(GLOB GLAD_SOURCES "${GLAD_OUT}/src/*.c")
add_library(glad_static STATIC ${GLAD_SOURCES})
target_include_directories(glad_static PUBLIC ${GLAD_OUT}/include)
add_library(glad INTERFACE)
add_library(glad::glad ALIAS glad)
target_link_libraries(glad INTERFACE glad_static)