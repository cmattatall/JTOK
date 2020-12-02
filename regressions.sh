#!/bin/bash

rm -r build
cmake -S . -B build
cmake --build build
pushd build
ctest -V
popd