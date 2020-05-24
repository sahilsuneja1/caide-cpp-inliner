cmake_minimum_required(VERSION 2.8.12)
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()
if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
project(CaideInliner)

option(CAIDE_USE_SYSTEM_CLANG "Use system-installed clang/llvm instead of compiling them from scratch" OFF)

# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project

if(CAIDE_USE_SYSTEM_CLANG)
    find_package(Clang REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
else()
    # set(LLVM_EXTERNAL_CLANG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/llvm-project/clang" CACHE PATH "" FORCE)
    set(LLVM_ENABLE_PROJECTS clang)
    set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "" FORCE)
    option(BUILD_SHARED_LIBS "" OFF)
    option(LLVM_BUILD_TOOLS "" OFF)
    option(LLVM_BUILD_EXAMPLES "" OFF)
    option(LLVM_BUILD_TESTS "" OFF)
    option(LLVM_BUILD_DOCS "" OFF)
    option(LLVM_ENABLE_DOXYGEN "" OFF)
    option(LLVM_ENABLE_SPHINX "" OFF)
    option(LLVM_ENABLE_TERMINFO "" OFF)
    option(LLVM_ENABLE_LIBEDIT "" OFF)
    option(LLVM_ENABLE_LIBPFM "" OFF)
    option(LLVM_ENABLE_LIBXML2 "" OFF)
    option(LLVM_ENABLE_OCAMLDOC "" OFF)
    option(LLVM_ENABLE_PLUGINS "" OFF)
    # Enable RTTI, even though it may decrease performance.
    # It's consistent with how LLVM is packaged in Debian and doesn't require -fno-rtti code for user code.
    option(LLVM_ENABLE_RTTI "" ON)
    option(LLVM_ENABLE_ZLIB "" OFF)
    option(LLVM_INCLUDE_BENCHMARKS "" OFF)
    option(LLVM_INCLUDE_DOCS "" OFF)
    option(LLVM_INCLUDE_EXAMPLES "" OFF)
    option(LLVM_INCLUDE_GOTESTS "" OFF)
    option(LLVM_INCLUDE_RUNTIMES "" OFF)
    option(LLVM_INCLUDE_TESTS "" OFF)
    option(LLVM_INCLUDE_TOOLS "" OFF)
    option(LLVM_INCLUDE_UTILS "" OFF)
    option(WITH_POLLY "" OFF)
    set(HAVE_LIBEDIT false CACHE BOOL "" FORCE)

    add_subdirectory(llvm-project/llvm EXCLUDE_FROM_ALL)

    if(POLICY CMP0024)
        cmake_policy(SET CMP0024 OLD)
    endif()
    include("${CMAKE_BINARY_DIR}/lib/cmake/clang/ClangConfig.cmake")

    # If the above doesn't work anymore, we'll probably need to hardcode certain variables:
    # set(LLVM_PACKAGE_VERSION "xx.x.x")
    # set(LLVM_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/llvm-project/llvm/lib")
    # set(LLVM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/llvm-project/llvm/include"
    #   "${CMAKE_CURRENT_BINARY_DIR}/llvm-project/llvm/include")
    # set(CLANG_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/llvm-project/clang/include"
    #     "${CMAKE_CURRENT_BINARY_DIR}/llvm-project/llvm/tools/clang/include")
    # set(LLVM_DEFINITIONS "")

    message(STATUS "Using LLVM from submodule ${LLVM_PACKAGE_VERSION}")
endif()

if(LLVM_PACKAGE_VERSION VERSION_LESS "3.8")
    message(WARNING "A version of clang/llvm that is too old is detected. Build failure is possible.")
endif()
if(LLVM_PACKAGE_VERSION VERSION_GREATER "10.0.255")
    message(WARNING "A version of clang/llvm that is too recent is detected. Build failure is possible.")
endif()

set(clang_libs clangAST clangBasic clangFrontend clangLex clangRewrite clangSema clangTooling)

message(STATUS "LLVM_LIBRARY_DIRS=${LLVM_LIBRARY_DIRS}")
message(STATUS "CLANG_LIBRARY_DIRS=${CLANG_LIBRARY_DIRS}")
message(STATUS "LLVM_INCLUDE_DIRS=${LLVM_INCLUDE_DIRS}")
message(STATUS "CLANG_INCLUDE_DIRS=${CLANG_INCLUDE_DIRS}")
message(STATUS "LLVM_DEFINITIONS=${LLVM_DEFINITIONS}")
message(STATUS "CLANG_DEFINITIONS=${CLANG_DEFINITIONS}")

# Find libraries corresponding to LLVM components that clang_libs (recursively) depend on.
# The result is in topological order.
# Equivalent of `llvm-config --link-static --libs <components>'.
# explicit_map_components_to_libraries(all_llvm_libs ${llvm_components})
# message(STATUS "LLVM libs: ${all_llvm_libs}")
# file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/caide-libs.txt" "${clang_libs};${all_llvm_libs}")

add_definitions(${LLVM_DEFINITIONS})


include(CheckCXXCompilerFlag)

function(add_compiler_option_if_supported opt)
    CHECK_CXX_COMPILER_FLAG("${opt}" compiler_supports_option)
    if(compiler_supports_option)
        add_compile_options("${opt}")
    endif()
endfunction(add_compiler_option_if_supported)

# CXX_STANDARD() is available only in cmake >= 3
# Set c++11 flag explicitly
# add_compiler_option_if_supported("-std=c++14")

if(MSVC)
    # Use the highest warning level for Visual Studio.
    add_compile_options("/W4")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    # Add warnings for g++
    foreach(flag -Wall -Wextra -Wshadow -Wlogical-op -Werror=return-type)
        add_compiler_option_if_supported("${flag}")
    endforeach()
endif()


set(inlinerSources caideInliner.cpp clang_compat.cpp DependenciesCollector.cpp inliner.cpp MergeNamespacesVisitor.cpp
    optimizer.cpp OptimizerVisitor.cpp RemoveInactivePreprocessorBlocks.cpp SmartRewriter.cpp
    SourceInfo.cpp SourceLocationComparers.cpp util.cpp)

add_library(caideInliner STATIC ${inlinerSources})

target_include_directories(caideInliner SYSTEM PRIVATE ${CLANG_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
target_link_libraries(caideInliner PRIVATE ${clang_libs})

add_subdirectory(cmd)

enable_testing()
add_subdirectory(test-tool)

