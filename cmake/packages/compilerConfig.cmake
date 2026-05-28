if (NOT SSE_MINIMAL)

# ---- create target ONCE ----
add_library(CLANG_IMPL INTERFACE)

# ---- LLVM components ----
set(LLVM_COMPONENTS
    OrcJIT
    Core
    Support
    X86CodeGen
    X86AsmParser
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include(FetchContent)

    set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "" FORCE)
    set(LLVM_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(LLVM_BUILD_UTILS OFF CACHE BOOL "" FORCE)
    set(LLVM_INCLUDE_TESTS OFF CACHE BOOL "" FORCE)
    set(LLVM_INCLUDE_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(LLVM_INCLUDE_BENCHMARKS OFF CACHE BOOL "" FORCE)
    set(LLVM_ENABLE_ZSTD OFF CACHE BOOL "" FORCE)
    set(LLVM_ENABLE_ZLIB OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        llvm_src
        URL https://github.com/llvm/llvm-project/releases/download/llvmorg-21.1.8/llvm-project-21.1.8.src.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SOURCE_SUBDIR llvm
    )
    FetchContent_MakeAvailable(llvm_src)

    llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_COMPONENTS})
    set(LLVM_INCLUDE_DIRS
        ${llvm_src_SOURCE_DIR}/llvm/include
        ${llvm_src_BINARY_DIR}/include
    )
else()
    find_package(LLVM 21.1 REQUIRED CONFIG)
    llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_COMPONENTS})
endif()

# ---- zstd ----
if (NOT TARGET zstd::libzstd_shared)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZSTD REQUIRED libzstd)
add_library(zstd::libzstd_shared INTERFACE IMPORTED GLOBAL)
set_target_properties(zstd::libzstd_shared PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ZSTD_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${ZSTD_LIBRARIES}"
)
endif()

# ---- LLVM setup ----
target_include_directories(CLANG_IMPL INTERFACE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(CLANG_IMPL INTERFACE ${LLVM_DEFINITIONS})
target_link_libraries(CLANG_IMPL INTERFACE
    ${LLVM_LIBS}
    zstd::libzstd_shared
)
add_library(CLANG::CLANG ALIAS CLANG_IMPL)

endif()