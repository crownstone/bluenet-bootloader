#!/bin/bash

usage() {
	echo "Usage: $0 <package_prefix>"
}

prepare() {
	echo "Preparing package for $1"
	make clean
	make VER=$1 DEFAULT_HARDWARE_BOARD=$3 package PACKAGE_PREFIX=$2 
}

if [ $# -ne 1 ]; then
	usage
	exit 1
fi

echo "Create both v1.8 and v1.9 for all default hardware boards."
prepare "1.8.0" $1 "ACR01B2C"
prepare "1.9.0" $1 "ACR01B2C"
prepare "1.8.1" $1 "GUIDESTONE"
prepare "1.9.1" $1 "GUIDESTONE"
