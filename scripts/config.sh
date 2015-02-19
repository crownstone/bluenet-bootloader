#!/bin/sh

if [ ! -d "$BLUENET_CONFIG_DIR" ]; then
	echo "ERROR: environment variable 'BLUENET_CONFIG_DIR' should be set to the dir containing \"CMakeBuild.config\""
	exit 1
fi

source ${BLUENET_CONFIG_DIR}/CMakeBuild.config
