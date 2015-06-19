#!/bin/sh

if [ -z $BLUENET_CONFIG_DIR ]; then
	echo "ERROR: environment variable 'BLUENET_CONFIG_DIR' should be set."
	exit 1
fi

source ${BLUENET_CONFIG_DIR}/CMakeBuild.config
