#!/bin/sh

if [ ! -d "$CROWNSTONE_DIR" ]; then
	echo "ERROR: environment variable 'CROWNSTONE_DIR' should be set to the dir containing \"CMakeBuild.config\""
	exit 1
fi

source ${CROWNSTONE_DIR}/CMakeBuild.config
