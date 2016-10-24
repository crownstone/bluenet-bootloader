#!/bin/bash

compilation_mode=release

path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $path/_utils.sh
source ${path}/_check_targets.sh

source ${path}/config.sh

objcopy=${COMPILER_PATH}/bin/${COMPILER_TYPE}-objcopy
objsize=${COMPILER_PATH}/bin/${COMPILER_TYPE}-size

prefix=dobots
device_variant=xxaa

output_path=${BLUENET_BUILD_DIR}/bootloader
mkdir -p "$output_path"

log "++ Go to \"${path}/../gcc\""
cd "${path}/../gcc"

make clean
