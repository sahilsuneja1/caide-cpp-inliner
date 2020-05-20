#!/bin/bash
set -ev

export CXX=g++-7
export CC=gcc-7

env
cmake --version
"$CXX" --version
"$CC" --version
date

git submodule update --init
if [ "$CAIDE_USE_SYSTEM_CLANG" = "ON" ]
then
    # Tell CMake where to look for LLVMConfig
    case "$CAIDE_CLANG_VERSION" in
        3.6|3.7|3.8)
            # CMake packaging for these is broken in Ubuntu
            export LLVM_DIR="$TRAVIS_BUILD_DIR/travis/cmake/$CAIDE_CLANG_VERSION/"
            ;;

        *)
            export LLVM_DIR=/usr/lib/llvm-$CAIDE_CLANG_VERSION/
            ;;
    esac
fi

date

mkdir build
cd build
cmake -DCAIDE_USE_SYSTEM_CLANG=$CAIDE_USE_SYSTEM_CLANG \
    -DCAIDE_CLANG_VERSION="$CAIDE_CLANG_VERSION" -DCMAKE_BUILD_TYPE=MinSizeRel ../src
# First build may run out of memory
make -j3 || make -j1

date

ctest --verbose

