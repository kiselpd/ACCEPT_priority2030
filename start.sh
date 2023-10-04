#!/bin/bash

function build {
    mkdir build
    cd build
    cmake ..
    cmake --build . -j
    cd ..
}

function run {
    cd build
    fuser -k 2000/tcp #NEED FIX
    fuser -k 3000/tcp #NEED FIX
    ./accept_server &
}

FILE_EXE=$(pwd)/build/accept_server

if ! [ -f "$FILE_EXE" ]
then
    build
fi

if [ -f "$FILE_EXE" ]
then
    run
fi