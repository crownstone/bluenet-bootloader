#!/bin/sh

# TODO: don't use BLUENET_DIR ?
if [ ! -d "${BLUENET_DIR}" ]; then
	echo "ERROR: environment variable 'BLUENET_DIR' should be set."
	exit 1
fi

if [ -e "${BLUENET_DIR}/CMakeBuild.config.default" ]; then
        echo "source ${BLUENET_DIR}/CMakeBuild.config.default"
        source "${BLUENET_DIR}/CMakeBuild.config.default"
fi

if [ -e "${BLUENET_DIR}/CMakeBuild.config.local" ]; then
        echo "source ${BLUENET_DIR}/CMakeBuild.config.local"
        source "${BLUENET_DIR}/CMakeBuild.config.local"
fi


if [ ! -d "$BLUENET_CONFIG_DIR" ]; then
	echo "ERROR: environment variable 'BLUENET_CONFIG_DIR' should be set."
	exit 1
fi

echo "COMPILER_PATH=${COMPILER_PATH}"

echo "source ${BLUENET_CONFIG_DIR}/CMakeBuild.config"
source "${BLUENET_CONFIG_DIR}/CMakeBuild.config"
