#!/bin/bash
set -ev

export CXX=g++-9
export CC=gcc-9
export CXXFLAGS=-std=c++14

env
cmake --version
"$CXX" --version
"$CC" --version
date

if [ "$CAIDE_USE_SYSTEM_CLANG" = "ON" ]
then
    # Tell CMake where to look for LLVMConfig
    case "$CAIDE_CLANG_VERSION" in
        3.6|3.7)
            # CMake packaging for these is broken in Ubuntu
            export LLVM_DIR="$TRAVIS_BUILD_DIR/travis/cmake/$CAIDE_CLANG_VERSION/"
            ;;

        *)
            export LLVM_DIR=/usr/lib/llvm-$CAIDE_CLANG_VERSION/
            ;;
    esac
fi

mkdir build
cd build
cmake -DCAIDE_USE_SYSTEM_CLANG=$CAIDE_USE_SYSTEM_CLANG \
    -DCAIDE_CLANG_VERSION="$CAIDE_CLANG_VERSION" -DCMAKE_BUILD_TYPE=MinSizeRel ../src
# First build may run out of memory
make -j3 || make -j1

date

# One of the tests requires clang/lib/Headers include directory
git submodule update --init --depth 1
date

ctest --verbose

