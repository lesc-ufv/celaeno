#!/usr/bin/env sh

set -e

rm -rf build

mkdir build && cd build

conan install .. --build missing && cd ..

reset && cmake -H. -Bbuild && cmake --build build
