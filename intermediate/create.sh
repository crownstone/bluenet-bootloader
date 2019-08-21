#!/bin/bash

# Usage: $ ./create.sh [package_name version]
# version = 1.8.0 or 1.9.0

VER_1_8=1.8.0
VER_1_9=1.9.0

prepare() {
    echo Preparing package for $1
    make clean
    make VER=$1 package PACKAGE_PREFIX=$2
}

if [ "$2" == $VER_1_8 ]; then
    echo Commanded v$VER_1_8
    prepare $VER_1_8 $1
elif [ "$2" == $VER_1_9 ]; then
    echo Commanded v$VER_1_9
    prepare $VER_1_9 $1
else
    echo Commanded both v$VER_1_8 and v$VER_1_9
    prepare $VER_1_8 $1
    prepare $VER_1_9 $1
fi