#!/bin/zsh

TYPE=$1

if [[ "${TYPE}" == "release" ]]; then
    CMAKE_CONFIG="Release"
elif [[ "${TYPE}" == "debug" ]]; then
    CMAKE_CONFIG="Debug"
fi

cmake -B build -S . -DCMAKE_BUILD_TYPE=$CMAKE_CONFIG
