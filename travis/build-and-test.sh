#!/bin/bash
set -ev

export CXX=g++-9
export CC=gcc-9
# export CXXFLAGS=-std=c++14

env | sort
cmake --version
"$CXX" --version
"$CC" --version
date

if [ "$CAIDE_USE_SYSTEM_CLANG" = "ON" ]
then
    # Debug
    llvm-config-"$CAIDE_CLANG_VERSION" --cxxflags --cflags --ldflags --has-rtti

    cxxver="-std=c++11"

    case "$CAIDE_CLANG_VERSION" in
        7)
            cxxver=""
            ;;
        10)
            cxxver="-std=c++14"
            ;;
    esac

    export CXXFLAGS="$CXXFLAGS $cxxver"
    export LLVM_ROOT=/usr/lib/llvm-$CAIDE_CLANG_VERSION
    export CLANG_ROOT=/usr/lib/llvm-$CAIDE_CLANG_VERSION
    export Clang_ROOT=/usr/lib/llvm-$CAIDE_CLANG_VERSION
    export LLVM_DIR=/usr/lib/llvm-$CAIDE_CLANG_VERSION/lib/cmake/llvm
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

