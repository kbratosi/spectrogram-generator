#!/bin/bash
clear
mkdir -p build > /dev/null
cd build
cmake .. $*
cd ..
cmake --build ./build