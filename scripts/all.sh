#!/bin/bash

compilation_mode=release
#compilation_mode=debug

path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. ${path}/check_targets.sh
source ${path}/config.sh

objcopy=${COMPILER_PATH}/bin/${COMPILER_TYPE}-objcopy
objsize=${COMPILER_PATH}/bin/${COMPILER_TYPE}-size

prefix=dobots
device_variant=xxaa

output_path=${BLUENET_CONFIG_DIR}/build
mkdir -p "$output_path"

echo "++ Go to \"${path}/../nrf51822_v6.0.0 - GCC_Bootloader/Board/nrf6310/device_firmware_updates/bootloader - gcc - BLE/gcc\""
cd "${path}/../nrf51822_v6.0.0 - GCC_Bootloader/Board/nrf6310/device_firmware_updates/bootloader - gcc - BLE/gcc"

cp gcc_nrf51_bootloader_xxaa.ld.in gcc_nrf51_bootloader_xxaa.ld
sed -i "s/@RAM_R1_BASE@/${RAM_R1_BASE}/" gcc_nrf51_bootloader_xxaa.ld
sed -i "s/@RAM_APPLICATION_AMOUNT@/${RAM_APPLICATION_AMOUNT}/" gcc_nrf51_bootloader_xxaa.ld
sed -i "s/@BOOTLOADER_START_ADDRESS@/${BOOTLOADER_START_ADDRESS}/" gcc_nrf51_bootloader_xxaa.ld
sed -i "s/@BOOTLOADER_LENGTH@/${BOOTLOADER_LENGTH}/" gcc_nrf51_bootloader_xxaa.ld

make $compilation_mode
if [ $? -ne 0 ]; then
	exit 1
fi

# this creates a _build directory
cd _build

# convert and/or rename to general names (.out is actually in .elf format)
echo "++ Convert binaries to formats convenient for inspection and upload"
$objcopy -j .text -j .data -O binary ${prefix}_bootloader_${device_variant}.out bootloader.bin
cp ${prefix}_bootloader_${device_variant}.out bootloader.elf
cp ${prefix}_bootloader_${device_variant}.hex bootloader.hex

$objsize bootloader.elf

# copy to target path
echo "++ Copy files to $output_path"
cp bootloader.bin $output_path
cp bootloader.elf $output_path
cp bootloader.hex $output_path
