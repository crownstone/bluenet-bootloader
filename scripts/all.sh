#!/bin/bash

source ${path}/_utils.sh

compilation_mode=release
#compilation_mode=debug

path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source ${path}/_check_targets.sh

# configure environment variables, load configuration files, check targets and
# assign serial_num from target
source ${path}/_config.sh

objcopy=${COMPILER_PATH}/bin/${COMPILER_TYPE}-objcopy
objsize=${COMPILER_PATH}/bin/${COMPILER_TYPE}-size

prefix=dobots
device_variant=xxaa

output_path=${BLUENET_BIN_DIR}
mkdir -p "$output_path"

log "++ Go to \"${path}/../gcc\""
cd "${path}/../gcc"

#cp gcc_nrf51_bootloader_xxaa.ld.in gcc_nrf51_bootloader_xxaa.ld
#sed -i "s/@RAM_R1_BASE@/${RAM_R1_BASE}/" gcc_nrf51_bootloader_xxaa.ld
#sed -i "s/@RAM_APPLICATION_AMOUNT@/${RAM_APPLICATION_AMOUNT}/" gcc_nrf51_bootloader_xxaa.ld
#sed -i "s/@BOOTLOADER_START_ADDRESS@/${BOOTLOADER_START_ADDRESS}/" gcc_nrf51_bootloader_xxaa.ld
#sed -i "s/@BOOTLOADER_LENGTH@/${BOOTLOADER_LENGTH}/" gcc_nrf51_bootloader_xxaa.ld
cp dfu_gcc_nrf52.ld.in dfu_gcc_nrf52.ld
sed -i "s/@RAM_R1_BASE@/${RAM_R1_BASE}/" dfu_gcc_nrf52.ld
sed -i "s/@RAM_APPLICATION_AMOUNT@/${RAM_APPLICATION_AMOUNT}/" dfu_gcc_nrf52.ld
sed -i "s/@BOOTLOADER_START_ADDRESS@/${BOOTLOADER_START_ADDRESS}/" dfu_gcc_nrf52.ld
sed -i "s/@BOOTLOADER_LENGTH@/${BOOTLOADER_LENGTH}/" dfu_gcc_nrf52.ld

#make $compilation_mode
make nrf52832_xxaa_s132
checkError "Error building bootloader"

# this creates a _build directory
cd _build

# convert and/or rename to general names (.out is actually in .elf format)
log "++ Convert binaries to formats convenient for inspection and upload"
#$objcopy -j .text -j .data -O binary ${prefix}_bootloader_${device_variant}.out bootloader.bin
#$objcopy -j .text -j .data -O ihex ${prefix}_bootloader_${device_variant}.out bootloader_dfu.hex
#cp ${prefix}_bootloader_${device_variant}.out bootloader.elf
#cp ${prefix}_bootloader_${device_variant}.hex bootloader.hex
$objcopy -j .text -j .data -O binary nrf52832_xxaa_s132.out bootloader.bin
$objcopy -j .text -j .data -O ihex nrf52832_xxaa_s132.out bootloader_dfu.hex
cp nrf52832_xxaa_s132.out bootloader.elf
cp nrf52832_xxaa_s132.hex bootloader.hex

$objsize bootloader.elf

# copy to target path
log "++ Copy files to $output_path"
cp bootloader.bin $output_path
cp bootloader.elf $output_path
cp bootloader.hex $output_path
cp bootloader_dfu.hex $output_path
