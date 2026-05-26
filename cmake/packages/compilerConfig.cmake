if (NOT SSE_MINIMAL)
find_package(LLVM 20.1 REQUIRED CONFIG)

add_library(CLANG_IMPL INTERFACE)

llvm_map_components_to_libnames(LLVM_LIBS
    OrcJIT
)

target_link_libraries(CLANG_IMPL INTERFACE
    LLVMX86CodeGen
    LLVMX86AsmParser
)


# LLVM part
target_include_directories(CLANG_IMPL INTERFACE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(CLANG_IMPL INTERFACE ${LLVM_DEFINITIONS})
target_link_libraries(CLANG_IMPL INTERFACE ${LLVM_LIBS})

add_library(CLANG::CLANG ALIAS CLANG_IMPL)
endif()