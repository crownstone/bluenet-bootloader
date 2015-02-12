#!/bin/sh

if [ ! -d "$BLUENET_DIR" ]; then
	echo "ERROR: environment variable 'BLUENET_DIR' should be set to the dir containing \"CMakeBuild.config\""
	exit 1
fi

source ${BLUENET_DIR}/CMakeBuild.config
