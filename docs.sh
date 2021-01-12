#!/usr/bin/env bash
#cd docs
#doxygen spectrogramDox

./build.sh -DBUILD_DOCS=TRUE 
cd ./build
make docs

