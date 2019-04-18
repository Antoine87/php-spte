#!/usr/bin/env sh

mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
rm -Rf build
