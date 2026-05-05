# include(ExternalProject)

# set(clangBuild "${CMAKE_BINARY_DIR}/llvm")
# set(OUT_BIN ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin-tools)
# set(OUT_LIB ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/lib)

# if(CMAKE_BUILD_TYPE STREQUAL "Release")
#     set(SIZE_FLAGS "-w -Os -flto -fdata-sections -ffunction-sections -fno-omit-frame-pointer")
# else()
#     set(SIZE_FLAGS "-w -O1 -g -fno-omit-frame-pointer")
# endif()

# ExternalProject_Add(llvm
#     SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/llvm-project/llvm
#     BINARY_DIR ${clangBuild}

#     CMAKE_ARGS
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#         -DLLVM_ENABLE_PROJECTS=clang
#         -DLLVM_TARGETS_TO_BUILD=Native
#         -DLLVM_ENABLE_ASSERTIONS=OFF
#         -DLLVM_INCLUDE_TESTS=OFF
#         -DLLVM_INCLUDE_EXAMPLES=OFF
#         -DLLVM_INCLUDE_DOCS=OFF
#         -DLLVM_ENABLE_BINDINGS=OFF
#         -DLLVM_BUILD_TOOLS=ON
#         -DLLVM_BUILD_UTILS=OFF
#         -DLLVM_BUILD_EXAMPLES=OFF
#         -DLLVM_BUILD_DOCS=OFF
#         -DLLVM_USE_LINKER=lld
#         -DLLVM_ENABLE_LTO=ON
#         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
#         -DCMAKE_C_FLAGS=${SIZE_FLAGS}
#         -DCMAKE_CXX_FLAGS=${SIZE_FLAGS}

#     INSTALL_COMMAND ""
#     LOG_BUILD ON
#     LOG_CONFIGURE ON
#     LOG_OUTPUT_ON_FAILURE ON
#     USES_TERMINAL_BUILD TRUE
#     USES_TERMINAL_CONFIGURE TRUE
# )

# # add_custom_target(copy_clang ALL
# #     COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_BIN}
# #     COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_LIB}

# #     # copy slangc
# #     COMMAND ${CMAKE_COMMAND} -E copy_if_different
# #         ${slangBuild}/${CMAKE_BUILD_TYPE}/bin/slangc
# #         ${OUT_BIN}/

# #     # copy only slang compiler libs
# #     COMMAND ${CMAKE_COMMAND} -E copy_if_different
# #         ${slangBuild}/${CMAKE_BUILD_TYPE}/lib/libslang-compiler*
# #         ${OUT_LIB}/
# # )
# # add_dependencies(copy_slang llvm)