#!/bin/bash

compilation_mode=release

path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source ${path}/config.sh

objcopy=${COMPILER_PATH}/bin/${COMPILER_TYPE}-objcopy
objsize=${COMPILER_PATH}/bin/${COMPILER_TYPE}-size

prefix=dobots
device_variant=xxaa

output_path=${BLUENET_DIR}/build
mkdir -p "$output_path"

echo "++ Go to \"${path}/../nrf51822_v6.0.0 - GCC_Bootloader/Board/nrf6310/device_firmware_updates/bootloader - gcc - BLE/gcc\""
cd "${path}/../nrf51822_v6.0.0 - GCC_Bootloader/Board/nrf6310/device_firmware_updates/bootloader - gcc - BLE/gcc"

make clean
