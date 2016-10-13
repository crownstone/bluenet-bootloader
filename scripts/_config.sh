#!/bin/sh

source "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/_utils.sh"

if [ ! -d "${BLUENET_DIR}" ]; then

	if [ -d "${BLUENET_WORKSPACE_DIR}" ]; then
		BLUENET_DIR=${BLUENET_WORKSPACE_DIR}/bluenet
	else
		err "ERROR: could not find environment variables BLUENET_DIR or BLUENET_WORKSPACE_DIR"
		err "Need to define at least one of the two!!"
		# err "ERROR: environment variable 'BLUENET_DIR' should be set."
		exit 1
	fi
fi

# config script expects target as second parameter, but bootloader scripts have the target
# as first parameter
source ${BLUENET_DIR}/scripts/_config.sh $1 $1

log "COMPILER_PATH=${COMPILER_PATH}"

log "source ${BLUENET_CONFIG_DIR}/CMakeBuild.config"
