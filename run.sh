#!/bin/bash

function clean {
    rm -r build
}

function build {
    cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=~/Apps/vcpkg/scripts/buildsystems/vcpkg.cmake
    cd build
    make
    cmake .. 
    cd ..
}

function rebuild {
    clean
    build
}

function run {
    build
    ./build/app 10
}

option=$1

echo "Operation: " $option

if [ $# -lt 1 ];
then 
    echo "No arguments supplied"
    echo "Usage: $0 <action>"
    echo "Actions: build, run, test"
    exit -1 
elif [ $option == "clean" ];
then
    clean
elif [ $option == "build" ];
then
    build
elif [ $option == "rebuild" ];
then
    rebuild 
elif [ $option == "run" ];
then
    run
else
    echo "Unknown argument"
    exit -1
fi
