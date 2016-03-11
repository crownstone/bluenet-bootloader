#!/bin/bash

compilation_mode=release

path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source ${path}/config.sh

objcopy=${COMPILER_PATH}/bin/${COMPILER_TYPE}-objcopy
objsize=${COMPILER_PATH}/bin/${COMPILER_TYPE}-size

prefix=dobots
device_variant=xxaa

output_path=${BLUENET_CONFIG_DIR}/build
mkdir -p "$output_path"

echo "++ Go to \"${path}/../src/gcc\""
cd "${path}/../src/gcc"

make clean
